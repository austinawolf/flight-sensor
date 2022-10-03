import logging
from enum import Enum

from blatann.services import ble_data_types
from blatann.services.ble_data_types import BleDataStream
from flight_recorder.packets import Packet, PacketType

logger = logging.getLogger(__name__)


class Opcode(Enum):
    GET_STATUS = 0
    STREAM = 1
    RECORD = 2
    PLAYBACK = 3
    STOP = 4
    CALIBRATE = 5
    ERROR = 6


class Response(Packet):
    TYPE = PacketType.RESPONSE

    @classmethod
    def decode(cls, stream: BleDataStream):
        packet_type = stream.decode(ble_data_types.Uint8)
        opcode = stream.decode(ble_data_types.Uint8)
        status = stream.decode(ble_data_types.Uint8)

        if packet_type != cls.TYPE.value:
            raise Exception

        return cls(Opcode(opcode), status)

    def __init__(self, opcode: Opcode, status):
        self._opcode = opcode
        self.status = status

    @property
    def opcode(self) -> Opcode:
        return self._opcode

    def encode(self) -> BleDataStream:
        raise NotImplementedError

    @property
    def is_error(self) -> bool:
        return self.status != 0
