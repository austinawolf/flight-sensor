import logging
from enum import Enum

logger = logging.getLogger(__name__)


class CommandType(Enum):
    GET_STATUS = 0
    STREAM = 1
    RECORD = 2
    PLAYBACK = 3
    STOP = 4
    CALIBRATE = 5


class Command:
    @classmethod
    def get_status(cls):
        return cls(CommandType.GET_STATUS)

    @classmethod
    def stream(cls, rate, flags, sampling_time):
        return cls(CommandType.STREAM, rate=rate, flags=flags, sampling_time=sampling_time)

    @classmethod
    def record(cls, rate, flags, sampling_time):
        return cls(CommandType.RECORD, rate=rate, flags=flags, sampling_time=sampling_time)

    @classmethod
    def playback(cls):
        return cls(CommandType.PLAYBACK)

    @classmethod
    def stop(cls):
        return cls(CommandType.STOP)

    @classmethod
    def calibrate(cls):
        return cls(CommandType.CALIBRATE)

    def __init__(self, command_type, rate=None, flags=None, sampling_time=None):
        self.type = command_type
        self.rate = rate
        self.flags = flags
        self.sampling_time = sampling_time
