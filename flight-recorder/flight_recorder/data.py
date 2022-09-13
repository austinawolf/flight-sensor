import binascii
import logging
import struct
from enum import Enum

logger = logging.getLogger(__name__)


class CommandCodes(Enum):
    GET_STATUS = 0
    START_SAMPLING = 1
    STOP_SAMPLING = 2
    START_PLAYBACK = 3
    STOP_PLAYBACK = 4
    CALIBRATE = 5
    ERROR = 6


class Command:
    PREAMBLE = 0xAA

    def __init__(self):
        self._response = None

    def to_bytes(self) -> bytes:
        raise NotImplementedError

    def _on_response(self, characteristic, event_args):
        logger.info("Response Received: [{}]".format(binascii.hexlify(event_args.value)))
        try:
            self._response = Response.from_bytes(event_args.value)
        except Exception as e:
            logger.error("Failed to decode Response, stream: [{}]".format(binascii.hexlify(event_args.value)))
            logger.exception(e)

    def send(self, char):
        # send command
        char.subscribe(self._on_response)
        logger.info("Sending Command: [{}]".format(binascii.hexlify(self.to_bytes())))
        char.write(self.to_bytes())

        # wait for response
        while not self._response:
            pass

        return self._response


class Data:
    FORMAT = "<BB"
    PREAMBLE = 0xBB

    @classmethod
    def from_bytes(cls, buffer):
        args = struct.unpack(cls.FORMAT, buffer)
        preamble, code = args
        if preamble != cls.PREAMBLE:
            raise Exception
        return cls(code)

    def __init__(self, code):
        self.code = code
