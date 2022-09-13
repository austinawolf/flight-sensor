import struct

from flight_recorder.commands import Command, CommandCodes


class StopSamplingCommand(Command):
    CODE = CommandCodes.STOP_SAMPLING
    FORMAT = "<BB"

    def to_bytes(self) -> bytes:
        return struct.pack(self.FORMAT,
                           self.PREAMBLE,
                           self.CODE.value)
