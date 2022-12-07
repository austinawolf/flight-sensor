import logging

from flight_recorder.services.imu.types.command import CommandType

logger = logging.getLogger(__name__)


class Response:
    @classmethod
    def get_status(cls, status):
        return cls(CommandType.GET_STATUS, status=status)

    @classmethod
    def stream(cls, status):
        return cls(CommandType.STREAM, status=status)

    @classmethod
    def record(cls, status):
        return cls(CommandType.RECORD, status=status)

    @classmethod
    def playback(cls, status):
        return cls(CommandType.PLAYBACK, status=status)

    @classmethod
    def stop(cls, status):
        return cls(CommandType.STOP, status=status)

    @classmethod
    def calibrate(cls, status):
        return cls(CommandType.CALIBRATE, status=status)

    def __init__(self, command_type, status=None):
        self.command_type = command_type
        self.status = status
