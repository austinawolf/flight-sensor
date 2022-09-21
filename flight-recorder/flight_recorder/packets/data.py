import binascii
import logging
import struct
from enum import Enum

logger = logging.getLogger(__name__)


class Data:
    FORMAT = "<BHIBllllhhhhhhhhh"
    PREAMBLE = 0xDD

    @classmethod
    def from_bytes(cls, buffer):
        args = struct.unpack(cls.FORMAT, buffer)

        preamble, index, \
        timestamp, \
        flags, \
        q0, q1, q2, q3, \
        g0, g1, g2, \
        a0, a1, a2, \
        c0, c1, c2 = args

        if preamble != cls.PREAMBLE:
            raise Exception

        return cls(flags, index, timestamp,
                   [q0, q1, q2, q3],
                   [g0, g1, g2],
                   [a0, a1, a2],
                   [c0, c1, c2])

    def __init__(self, flags, index, timestamp, quat, gyro, accel, compass):
        self.flags = flags
        self.index = index
        self.timestamp = timestamp
        self.quat = quat
        self.gyro = gyro
        self.accel = accel
        self.compass = compass

    def __repr__(self):
        return f"timestamp={self.timestamp}, " \
               f"index={self.index}, flags={self.flags}, " \
               f"quat={self.quat}, gyro={self.gyro}, " \
               f"accel={self.accel}, compass{self.compass}"

    def __str__(self):
        return self.__repr__()
