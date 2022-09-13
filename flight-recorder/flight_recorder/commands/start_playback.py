import struct

from flight_recorder.commands import Command, CommandCodes


class StartPlaybackCommand(Command):
    CODE = CommandCodes.START_PLAYBACK
    FORMAT = "<BB"

    def to_bytes(self) -> bytes:
        return struct.pack(self.FORMAT,
                           self.PREAMBLE,
                           self.CODE.value)
