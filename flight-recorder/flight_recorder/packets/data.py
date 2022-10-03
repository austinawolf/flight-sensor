from enum import Enum
from blatann.services import ble_data_types
from blatann.services.ble_data_types import BleDataStream
from flight_recorder.packets import Packet, PacketType


class Data(Packet):
    TYPE = PacketType.DATA

    @classmethod
    def decode(cls, stream: BleDataStream):
        packet_type = stream.decode(ble_data_types.Uint8)
        index = stream.decode(ble_data_types.Uint16)
        timestamp = stream.decode(ble_data_types.Uint32)
        flags = stream.decode(ble_data_types.Uint8)
        q0, q1, q2, q3 = stream.decode_multiple(ble_data_types.Int32, ble_data_types.Int32, ble_data_types.Int32, ble_data_types.Int32)
        g0, g1, g2 = stream.decode_multiple(ble_data_types.Int16, ble_data_types.Int16, ble_data_types.Int16)
        a0, a1, a2 = stream.decode_multiple(ble_data_types.Int16, ble_data_types.Int16, ble_data_types.Int16)
        c0, c1, c2 = stream.decode_multiple(ble_data_types.Int16, ble_data_types.Int16, ble_data_types.Int16)

        if packet_type != cls.TYPE.value:
            raise Exception

        return cls(flags, index, timestamp,
                   (q0, q1, q2, q3),
                   (g0, g1, g2),
                   (a0, a1, a2),
                   (c0, c1, c2))

    def __init__(self, flags, index, timestamp, quat, gyro, accel, compass):
        self.flags = flags
        self.index = index
        self.timestamp = timestamp
        self.quat = quat
        self.gyro = gyro
        self.accel = accel
        self.compass = compass

    def __repr__(self):
        return f"timestamp={self.timestamp}, " \
               f"index={self.index}, flags={self.flags}, " \
               f"quat={self.quat}, gyro={self.gyro}, " \
               f"accel={self.accel}, compass{self.compass}"

    def __str__(self):
        return self.__repr__()

    def encode(self) -> BleDataStream:
        raise NotImplementedError
