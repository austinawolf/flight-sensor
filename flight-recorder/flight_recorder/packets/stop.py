import struct

from flight_recorder.packets import Command, CommandCodes


class StopCommand(Command):
    CODE = CommandCodes.STOP
    FORMAT = "<BB"

    def to_bytes(self) -> bytes:
        return struct.pack(self.FORMAT,
                           self.PREAMBLE,
                           self.CODE.value)
