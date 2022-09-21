import binascii
import logging
from blatann import BleDevice
from blatann.event_type import EventSource
from blatann.uuid import Uuid128, Uuid16

from flight_recorder.packets import SessionStates
from flight_recorder.packets.calibrate import CalibrateCommand
from flight_recorder.packets.playback import PlaybackCommand
from flight_recorder.packets.record import RecordCommand
from flight_recorder.packets.state_update import StateUpdate
from flight_recorder.packets.stop import StopCommand
from flight_recorder.packets.stream import StreamCommand
from flight_recorder.packets.data import Data

logger = logging.getLogger(__name__)


class Client:
    DEVICE_NAME = "Flight Sensor"

    class UUIDs:
        IMU_SERVICE = Uuid128("924418ff-c32c-40ca-b9a9-d9024bf8b003")
        COMMAND_CHARACTERISTIC = Uuid16("1770")  # Uuid128("d9541770-a7c0-4a17-a548-1e0fa159ad01")
        DATA_CHARACTERISTIC = Uuid16("2aff")  # Uuid128("43192aff-8264-41bb-b660-678c8ec91201")

    def __init__(self, ble_device: BleDevice):
        """
        :type gattc_service: blatann.gatt.gattc.GattcService
        """
        self._on_data = EventSource("IMU Data Event")
        self._on_response = EventSource("Response Event")
        self._ble_device = ble_device
        self._peer = None
        self._service = None
        self._command_characteristic = None
        self._data_characteristic = None
        self._data = []
        self._state = SessionStates.IDLE

    @property
    def current_state(self):
        return self._state

    def _on_state_update(self, current, previous):
        logger.info(f"State Update: {previous.name} -> {current.name}")
        self._state = current

    def _on_data_recieved(self, characteristic, event_args):
        try:
            data = Data.from_bytes(event_args.value)
            logger.info(data)
            self._data.append(data)
        except Exception as e:
            logger.error("Failed to decode data, stream: [{}]".format(binascii.hexlify(event_args.value)))
            logger.exception(e)

    def _on_command_response(self, characteristic, event_args):
        try:
            preamble = event_args.value[0]
            if preamble == 0xCC:
                state_update = StateUpdate.from_bytes(event_args.value)
                self._on_state_update(state_update.current, state_update.previous)
            elif preamble == 0xBB:
                logger.info("Response")
            else:
                return
        except Exception as e:
            logger.error("Failed to decode data, stream: [{}]".format(binascii.hexlify(event_args.value)))
            logger.exception(e)

    def initialize(self):
        logger.info("Scanning for '{}'".format(self.DEVICE_NAME))

        target_address = None
        for report in self._ble_device.scanner.start_scan().scan_reports:
            if report.advertise_data.local_name == self.DEVICE_NAME:
                target_address = report.peer_address
                break

        if not target_address:
            logger.info("Did not find target peripheral")
            raise Exception

        # Initiate the connection and wait for it to finish
        logger.info("Found match: connecting to address {}".format(target_address))
        waitable = self._ble_device.connect(target_address)
        self._ble_device.connecting_peripheral.preferred_mtu_size = self._ble_device.max_mtu_size
        self._peer = waitable.wait()

        if not self._peer:
            logger.warning("Timed out connecting to device")
            return
        logger.info("Connected, conn_handle: {}".format(self._peer.conn_handle))

        logger.info("Update MTU")
        self._peer.exchange_mtu(self._peer.max_mtu_size).wait()

        # Wait up to 10 seconds for service discovery to complete
        _, event_args = self._peer.discover_services().wait(10, exception_on_timeout=False)
        logger.info("Service discovery complete! status: {}".format(event_args.status))

        # Find Characteristics
        self._service = self._peer.database.find_service(self.UUIDs.IMU_SERVICE)
        self._command_characteristic = self._peer.database.find_characteristic(self.UUIDs.COMMAND_CHARACTERISTIC)
        self._data_characteristic = self._peer.database.find_characteristic(self.UUIDs.DATA_CHARACTERISTIC)

        # Subscribe to Data Characteristic
        self._data_characteristic.subscribe(self._on_data_recieved)
        self._command_characteristic.subscribe(self._on_command_response)

    def deinitialize(self):
        logger.info("Disconnecting from peripheral")
        if self._peer:
            self._peer.disconnect().wait()

    def stream(self, rate: int, flags: int, session_time: int):
        return StreamCommand(rate, flags, session_time).send(self._command_characteristic)

    def record(self, rate: int, flags: int, stream_enable: bool, session_time: int):
        return RecordCommand(rate, flags, stream_enable, session_time).send(self._command_characteristic)

    def playback(self):
        return PlaybackCommand().send(self._command_characteristic)

    def stop(self):
        return StopCommand().send(self._command_characteristic)

    def calibrate(self):
        return CalibrateCommand().send(self._command_characteristic)
