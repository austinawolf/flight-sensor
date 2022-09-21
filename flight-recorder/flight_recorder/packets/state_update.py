import binascii
import logging
import struct
from enum import Enum

from flight_recorder.packets import SessionStates

logger = logging.getLogger(__name__)


class StateUpdate:
    FORMAT = "<BBB"
    PREAMBLE = 0xCC

    @classmethod
    def from_bytes(cls, buffer):
        args = struct.unpack(cls.FORMAT, buffer)
        preamble, current, previous = args

        if preamble != cls.PREAMBLE:
            raise Exception

        return cls(current, previous)

    def __init__(self, current, previous):
        self.current: SessionStates = SessionStates(current)
        self.previous: SessionStates = SessionStates(previous)

    def __repr__(self):
        return f"current={self.current}, " \
               f"previous={self.previous}"

    def __str__(self):
        return self.__repr__()
