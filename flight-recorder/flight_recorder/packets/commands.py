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


class Command(Packet):
    TYPE = PacketType.COMMAND

    @classmethod
    def decode(cls, stream: BleDataStream):
        raise NotImplementedError

    @property
    def opcode(self) -> Opcode:
        raise NotImplementedError

    def encode(self) -> BleDataStream:
        stream = BleDataStream()
        stream.encode(ble_data_types.Uint8, self.TYPE.value)
        stream.encode(ble_data_types.Uint8, self.opcode.value)
        return stream


class Stream(Command):
    @classmethod
    def decode(cls, stream: BleDataStream):
        raise NotImplementedError

    def __init__(self, rate, flags, sampling_time):
        self.rate = rate
        self.flags = flags
        self.sampling_time = sampling_time

    @property
    def opcode(self) -> Opcode:
        return Opcode.STREAM

    def encode(self) -> BleDataStream:
        stream = BleDataStream()
        stream.encode(ble_data_types.Uint8, self.TYPE.value)
        stream.encode(ble_data_types.Uint8, self.opcode.value)
        stream.encode(ble_data_types.Uint8, self.rate.value)
        stream.encode(ble_data_types.Uint8, self.flags)
        stream.encode(ble_data_types.Uint16, self.sampling_time)
        return stream


class Record(Command):
    @classmethod
    def decode(cls, stream: BleDataStream):
        raise NotImplementedError

    def __init__(self, rate, flags, stream_enable, sampling_time):
        self.rate = rate
        self.flags = flags
        self.stream_enable = stream_enable
        self.sampling_time = sampling_time

    @property
    def opcode(self) -> Opcode:
        return Opcode.RECORD

    def encode(self) -> BleDataStream:
        stream = BleDataStream()
        stream.encode(ble_data_types.Uint8, self.TYPE.value)
        stream.encode(ble_data_types.Uint8, self.opcode.value)
        stream.encode(ble_data_types.Uint8, self.rate.value)
        stream.encode(ble_data_types.Uint8, self.flags)
        # stream.encode(ble_data_types.Uint8, 1 if self.stream_enable else 0)
        stream.encode(ble_data_types.Uint16, self.sampling_time)
        return stream


class Playback(Command):
    @classmethod
    def decode(cls, stream: BleDataStream):
        raise NotImplementedError

    @property
    def opcode(self) -> Opcode:
        return Opcode.PLAYBACK


class Stop(Command):
    @classmethod
    def decode(cls, stream: BleDataStream):
        raise NotImplementedError

    @property
    def opcode(self) -> Opcode:
        return Opcode.STOP


class Calibrate(Command):
    @classmethod
    def decode(cls, stream: BleDataStream):
        raise NotImplementedError

    @property
    def opcode(self) -> Opcode:
        return Opcode.CALIBRATE
