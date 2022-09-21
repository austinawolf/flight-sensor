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


class BleImuService:
    DEVICE_NAME = "Flight Sensor"

    class UUIDs:
        IMU_SERVICE = Uuid128("924418ff-c32c-40ca-b9a9-d9024bf8b003")
        COMMAND_CHARACTERISTIC = Uuid16("1770")  # Uuid128("d9541770-a7c0-4a17-a548-1e0fa159ad01")
        DATA_CHARACTERISTIC = Uuid16("2aff")  # Uuid128("43192aff-8264-41bb-b660-678c8ec91201")

    def __init__(self):
        """
        :type gattc_service: blatann.gatt.gattc.GattcService
        """
        self._on_data = EventSource("IMU Data Event")
        self._on_response = EventSource("Response Event")
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

    def initialize(self, peer):
        # Find Characteristics
        self._service = peer.database.find_service(self.UUIDs.IMU_SERVICE)
        self._command_characteristic = peer.database.find_characteristic(self.UUIDs.COMMAND_CHARACTERISTIC)
        self._data_characteristic = peer.database.find_characteristic(self.UUIDs.DATA_CHARACTERISTIC)

        # Subscribe to Data Characteristic
        self._data_characteristic.subscribe(self._on_data_recieved)
        self._command_characteristic.subscribe(self._on_command_response)

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
