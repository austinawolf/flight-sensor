import logging
from enum import Enum
from blatann.services.ble_data_types import BleDataStream


logger = logging.getLogger(__name__)


class PacketType(Enum):
    COMMAND = 0xAA
    RESPONSE = 0xBB
    STATE_UPDATE = 0xCC
    DATA = 0xDD


class Packet:
    @property
    def TYPE(cls) -> PacketType:
        raise NotImplementedError

    @classmethod
    def decode(cls, stream: BleDataStream):
        raise NotImplementedError

    def encode(self) -> BleDataStream:
        raise NotImplementedError
