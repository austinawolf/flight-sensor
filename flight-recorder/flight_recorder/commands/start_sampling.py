import struct

from flight_recorder.commands import Command, CommandCodes


class StartSamplingCommand(Command):
    CODE = CommandCodes.START_SAMPLING
    FORMAT = "<BBBBBB"

    def __init__(self, rate, flags, destination, sampling_time):
        super().__init__()
        self.rate = rate
        self.flags = flags
        self.destination = destination
        self.sampling_time = sampling_time

    def to_bytes(self) -> bytes:
        return struct.pack(self.FORMAT,
                           self.PREAMBLE,
                           self.CODE.value,
                           self.rate,
                           self.flags,
                           self.destination,
                           self.sampling_time)
