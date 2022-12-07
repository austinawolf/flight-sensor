import binascii
import logging
from typing import List, Callable, Any

from rx.subject import BehaviorSubject

from flight_recorder.services.imu.transport import TransportLayerBase, ResponseWaitable
from flight_recorder.services.imu.transport.ble_imu_encode import decode_message, encode_message
from flight_recorder.services.imu.transport.message import Message, MessageType

logger = logging.getLogger(__name__)


class Transaction:
    def __init__(self, command: Message, handler: Callable[[bytes], Any]):
        self.command = command
        self.handler = handler


class BleImuTransport(TransportLayerBase):
    def __init__(self, characteristic):
        self._characteristic = characteristic
        self.pending_commands: List[Message] = []
        self._sequence_number = 1
        self._characteristic.subscribe(self._on_write)
        self._on_notification = BehaviorSubject(None)

    def _on_write(self, char, event_args):
        logger.info(f"RX: {binascii.hexlify(event_args.value)}")
        message = decode_message(event_args.value)
        if message.type == MessageType.RESPONSE:
            self._on_response(message)
        elif message.type == MessageType.UPDATE:
            self._on_notification.on_next(message.payload)
        else:
            pass

    def _write(self, stream):
        logger.info(f"TX: {binascii.hexlify(stream)}")
        self._characteristic.write(stream)

    def _next_sequence_number(self) -> int:
        value = self._sequence_number
        self._sequence_number += 1
        return value

    def _on_response(self, response: Message):
        for command in self.pending_commands:
            if command.sequence_number == response.sequence_number:
                self.pending_commands.remove(command)
                command.on_response(response.payload)
                return

    def _add_to_pending(self, message: Message):
        self.pending_commands.append(message)

    def _send_message(self, message):
        if message.type == MessageType.COMMAND:
            self._add_to_pending(message)

            # encode message
            stream = encode_message(message)

            # send message
            self._write(stream)
        else:
            logger.warning(f"Unsupported Message Type: {message.type.name}")

    def send_command(self, command_payload: bytes, on_response: Callable[[bytes], None]):
        sequence_number = self._next_sequence_number()

        message = Message(MessageType.COMMAND, command_payload, sequence_number)
        message.on_response = on_response

        self._send_message(message)

    @property
    def on_update(self):
        return self._on_notification
