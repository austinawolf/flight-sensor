from blatann.services import ble_data_types
from blatann.services.ble_data_types import BleDataStream

from flight_recorder.services.imu.types import SessionState
from flight_recorder.services.imu.types.command import Command, CommandType
from flight_recorder.services.imu.types.notification import Notification
from flight_recorder.services.imu.types.sample import Sample
from flight_recorder.services.imu.types.response import Response


def encode_command(command: Command) -> bytes:
    stream = BleDataStream()
    stream.encode(ble_data_types.Uint8, command.type.value)

    if command.type.value == CommandType.STREAM:
        stream.encode(ble_data_types.Uint8, command.rate.value)
        stream.encode(ble_data_types.Uint8, command.flags)
        stream.encode(ble_data_types.Uint16, command.sampling_time)
    elif command.type.value == CommandType.RECORD:
        stream.encode(ble_data_types.Uint8, command.rate.value)
        stream.encode(ble_data_types.Uint8, command.flags)
        stream.encode(ble_data_types.Uint16, command.sampling_time)

    return stream.value


def decode_response(payload: bytes) -> Response:
    stream = BleDataStream(value=payload)

    response_type = stream.decode(ble_data_types.Uint8)
    if response_type == CommandType.STREAM.value:
        status = stream.decode(ble_data_types.Uint8)
        return Response.stream(status)
    elif response_type == CommandType.RECORD.value:
        status = stream.decode(ble_data_types.Uint8)
        return Response.record(status)
    elif response_type == CommandType.PLAYBACK.value:
        status = stream.decode(ble_data_types.Uint8)
        return Response.playback(status)
    elif response_type == CommandType.STOP.value:
        status = stream.decode(ble_data_types.Uint8)
        return Response.stop(status)
    elif response_type == CommandType.CALIBRATE.value:
        status = stream.decode(ble_data_types.Uint8)
        return Response.calibrate(status)
    else:
        raise ValueError(response_type)


def decode_notification(payload: bytes) -> Notification:
    stream = BleDataStream(value=payload)

    notification_type = stream.decode(ble_data_types.Uint8)
    if notification_type == Notification.Type.STATE_UPDATE.value:
        current = stream.decode(ble_data_types.Uint8)
        previous = stream.decode(ble_data_types.Uint8)
        return Notification.state_update(SessionState(previous), SessionState(current))
    elif notification_type == Notification.Type.SAMPLE.value:
        timestamp = stream.decode(ble_data_types.Uint32)
        flags = stream.decode(ble_data_types.Uint8)
        ax, ay, az = stream.decode_multiple(ble_data_types.Int16, ble_data_types.Int16, ble_data_types.Int16)
        gx, gy, gz = stream.decode_multiple(ble_data_types.Int16, ble_data_types.Int16, ble_data_types.Int16)
        cx, cy, cz = stream.decode_multiple(ble_data_types.Int16, ble_data_types.Int16, ble_data_types.Int16)
        q0, q1, q2, q3 = stream.decode_multiple(ble_data_types.Int32, ble_data_types.Int32, ble_data_types.Int32, ble_data_types.Int32)
        sample = Sample(timestamp, flags, (ax, ay, az), (gx, gy, gz), (cx, cy, cz), (q0, q1, q2, q3))
        return Notification.sample(sample)
    else:
        raise ValueError(notification_type)
