import struct

from flight_recorder.packets import Command, CommandCodes


class PlaybackCommand(Command):
    CODE = CommandCodes.PLAYBACK
    FORMAT = "<BB"

    def to_bytes(self) -> bytes:
        return struct.pack(self.FORMAT,
                           self.PREAMBLE,
                           self.CODE.value)
