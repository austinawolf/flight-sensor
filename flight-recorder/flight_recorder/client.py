import binascii
import logging

from blatann import BleDevice
from blatann.event_type import EventSource, Event
from blatann.event_args import DecodedReadCompleteEventArgs, ReadCompleteEventArgs, GattOperationCompleteReason
from blatann.nrf import nrf_events
from blatann.uuid import Uuid128

logger = logging.getLogger(__name__)


class Client(object):
    DEVICE_NAME = "Flight Sensor"

    class UUIDs:
        IMU_SERVICE = Uuid128("924418ff-c32c-40ca-b9a9-d9024bf8b003")
        COMMAND_CHARACTERISTIC = Uuid128("d9541770-a7c0-4a17-a548-1e0fa159ad01")
        DATA_CHARACTERISTIC = Uuid128("43192aff-8264-41bb-b660-678c8ec91201")

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

    def initialize(self):
        self._ble_device.event_logger.suppress(nrf_events.GapEvtAdvReport)
        self._ble_device.open()
        self._ble_device.scanner.set_default_scan_params(timeout_seconds=10)

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
        self._peer = self._ble_device.connect(target_address).wait()
        if not self._peer:
            logger.warning("Timed out connecting to device")
            return
        logger.info("Connected, conn_handle: {}".format(self._peer.conn_handle))

        # Wait up to 10 seconds for service discovery to complete
        _, event_args = self._peer.discover_services().wait(10, exception_on_timeout=False)
        logger.info("Service discovery complete! status: {}".format(event_args.status))

        # Find Characteristics
        self._service = self._peer.database.find_service(self.UUIDs.IMU_SERVICE)
        self._command_characteristic = self._peer.database.find_characteristic(self.UUIDs.DATA_CHARACTERISTIC)
        self._data_characteristic = self._peer.database.find_characteristic(self.UUIDs.COMMAND_CHARACTERISTIC)

    def deinitialize(self):
        logger.info("Disconnecting from peripheral")
        if self._peer:
            self._peer.disconnect().wait()

    @property
    def on_data_received(self):  # -> Event[MotionClient, DecodedReadCompleteEventArgs[int]]:
        return self._on_data

    @property
    def on_response_received(self):  # -> Event[MotionClient, DecodedReadCompleteEventArgs[int]]:
        return self._on_response

    @property
    def can_enable_notifications(self) -> bool:
        return self._data_characteristic.subscribable and self._command_characteristic.subscribable

    def enable_notifications(self):
        self._command_characteristic.subscribe(self._on_data_notification)
        self._data_characteristic.subscribe(self._on_data_notification)

    def disable_notifications(self):
        self._command_characteristic.subscribe(self._on_data_notification)
        self._data_characteristic.subscribe(self._on_data_notification)

    def _on_data_notification(self, characteristic, event_args):
        decoded_value = None
        try:
            pass
        except Exception as e:
            logger.error("Failed to decode Battery Level, stream: [{}]".format(binascii.hexlify(event_args.value)))
            logger.exception(e)

        decoded_event_args = DecodedReadCompleteEventArgs.from_notification_complete_event_args(event_args, decoded_value)

        data = None #MotionData.from_bytes(decoded_event_args.value)
        self._on_data.notify(self, data)

    def _on_command_notification(self, characteristic, event_args):
        decoded_value = None
        try:
            pass
        except Exception as e:
            logger.error("Failed to decode Battery Level, stream: [{}]".format(binascii.hexlify(event_args.value)))
            logger.exception(e)

        decoded_event_args = DecodedReadCompleteEventArgs.from_notification_complete_event_args(event_args, decoded_value)
        response = None # MotionData.from_bytes(decoded_event_args.value)

        self._on_response.notify(self, response)

    def start_sampling(self):
        pass

    def stop_sampling(self):
        pass

    def start_playback(self):
        pass

    def stop_playback(self):
        pass

    def calibrate(self):
        pass
