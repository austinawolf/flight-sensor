from enum import Enum


class MessageType(Enum):
    COMMAND = 1
    RESPONSE = 2
    UPDATE = 3


class Message:
    def __init__(self, message_type: MessageType, payload, sequence_number):
        self.type = message_type
        self.payload = payload
        self.sequence_number = sequence_number
        self.on_response = None
