import struct

from flight_recorder.packets import Command, CommandCodes


class RecordCommand(Command):
    CODE = CommandCodes.RECORD
    FORMAT = "<BBBBBB"

    def __init__(self, rate, flags, stream_enable, sampling_time):
        super().__init__()
        self.rate = rate
        self.flags = flags
        self.stream_enable = stream_enable
        self.sampling_time = sampling_time

    def to_bytes(self) -> bytes:
        return struct.pack(self.FORMAT,
                           self.PREAMBLE,
                           self.CODE.value,
                           self.rate,
                           self.flags,
                           self.stream_enable,
                           self.sampling_time)
