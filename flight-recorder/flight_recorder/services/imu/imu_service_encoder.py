from blatann.services import ble_data_types
from blatann.services.ble_data_types import BleDataStream

from flight_recorder.services.imu.types.command import Command, CommandType
from flight_recorder.services.imu.types.response import Response


def encode_command(command: Command) -> bytes:
    stream = BleDataStream()
    stream.encode(ble_data_types.Uint8, command.type.value)

    # Stream
    stream.encode_if(command.type == CommandType.STREAM, ble_data_types.Uint8, command.rate.value)
    stream.encode_if(command.type == CommandType.STREAM, ble_data_types.Uint8, command.flags)
    stream.encode_if(command.type == CommandType.STREAM, ble_data_types.Uint16, command.sampling_time)

    return stream.value


def decode_response(payload: bytes) -> Response:
    stream = BleDataStream(value=payload)

    response_type = stream.decode(ble_data_types.Uint8)
    if response_type == CommandType.STREAM.value:
        status = stream.decode(ble_data_types.Uint8)
        return Response.stream(status)
    else:
        raise ValueError(response_type)
