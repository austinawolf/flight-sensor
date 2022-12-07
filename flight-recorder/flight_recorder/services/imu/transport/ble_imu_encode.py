from blatann.services import ble_data_types
from blatann.services.ble_data_types import BleDataStream

from flight_recorder.services.imu.transport.message import Message, MessageType


def encode_message(message: Message) -> bytes:
    stream = BleDataStream()
    stream.encode(ble_data_types.Uint8, message.type.value)
    stream.encode(ble_data_types.Uint8, message.sequence_number)
    stream.value += message.payload
    return stream.value


def decode_message(stream: bytes) -> Message:
    stream = BleDataStream(stream)
    message_type = stream.decode(ble_data_types.Uint8)
    sequence_number = stream.decode(ble_data_types.Uint8)
    payload = stream.take_all()
    return Message(MessageType(message_type), payload, sequence_number)
