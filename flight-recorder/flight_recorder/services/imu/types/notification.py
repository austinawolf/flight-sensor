import logging

from blatann.services.ble_data_types import BleDataStream

from flight_recorder.services.imu.types.command import CommandType

logger = logging.getLogger(__name__)


class Notification:
    @classmethod
    def decode(cls, stream: BleDataStream):
        pass
        return cls()

    def __init__(self, sample=None, previous_state=None, current_state=None):
        self.sample = sample
        self.previous_state = previous_state
        self.current_state = current_state
