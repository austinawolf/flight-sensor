import binascii
import logging
from blatann.event_type import EventSource, Event
from blatann.event_args import DecodedReadCompleteEventArgs, ReadCompleteEventArgs, GattOperationCompleteReason
from blatann.uuid import Uuid128
from blatann.waitables import EventWaitable

from motion_reader import MotionData

logger = logging.getLogger(__name__)

MOTION_SERVICE_UUID = Uuid128("924418ff-c32c-40ca-b9a9-d9024bf8b003")
MOTION_CHARACTERISTIC_UUID = Uuid128("43192aff-8264-41bb-b660-678c8ec91201")


class MotionClient(object):
    def __init__(self, gattc_service):
        """
        :type gattc_service: blatann.gatt.gattc.GattcService
        """
        self._service = gattc_service
        self._motion_characteristic = gattc_service.find_characteristic(MOTION_CHARACTERISTIC_UUID)
        self._on_motion_data_event = EventSource("Motion Data Event")

    @property
    def on_motion_data_received(self):  # -> Event[MotionClient, DecodedReadCompleteEventArgs[int]]:
        """
        Event that is generated whenever the battery level on the peripheral is updated, whether
        it is by notification or from reading the characteristic itself.
        The DecodedReadCompleteEventArgs value given is the integer battery percent received. If the read failed
        or failed to decode, the value will be equal to the raw bytes received.
        """
        return self._on_motion_data_event

    @property
    def can_enable_notifications(self) -> bool:
        """
        Checks if the battery level characteristic allows notifications to be subscribed to
        :return: True if notifications can be enabled, False if not
        """
        return self._motion_characteristic.subscribable

    def enable_notifications(self):
        """
        Enables notifications for the battery level characteristic. Note: this function will raise an exception
        if notifications aren't possible
        :return: a Waitable which waits for the write to finish
        """
        return self._motion_characteristic.subscribe(self._on_motion_notification)

    def disable_notifications(self):
        """
        Disables notifications for the battery level characteristic. Note: this function will raise an exception
        if notifications aren't possible
        :return: a Waitable which waits for the write to finish
        """
        return self._motion_characteristic.unsubscribe()

    def _on_motion_notification(self, characteristic, event_args):
        """
        :param characteristic:
        :type event_args: blatann.event_args.NotificationReceivedEventArgs
        :return:
        """
        decoded_value = None
        try:
            pass
        except Exception as e:
            logger.error("Failed to decode Battery Level, stream: [{}]".format(binascii.hexlify(event_args.value)))
            logger.exception(e)

        decoded_event_args = DecodedReadCompleteEventArgs.from_notification_complete_event_args(event_args, decoded_value)

        motion_data = MotionData.from_bytes(decoded_event_args.value)

        self._on_motion_data_event.notify(self, motion_data)

    @classmethod
    def find_in_database(cls, gattc_database):
        """
        :type gattc_database: blatann.gatt.gattc.GattcDatabase
        :rtype: BatteryClient
        """
        service = gattc_database.find_service(MOTION_SERVICE_UUID)
        if service:
            return MotionClient(service)
