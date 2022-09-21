import struct

from flight_recorder.packets import Command, CommandCodes


class StreamCommand(Command):
    CODE = CommandCodes.STREAM
    FORMAT = "<BBBBB"

    def __init__(self, rate, flags, sampling_time):
        super().__init__()
        self.rate = rate
        self.flags = flags
        self.sampling_time = sampling_time

    def to_bytes(self) -> bytes:
        return struct.pack(self.FORMAT,
                           self.PREAMBLE,
                           self.CODE.value,
                           self.rate,
                           self.flags,
                           self.sampling_time)
