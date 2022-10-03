from enum import Enum

from blatann.services import ble_data_types
from blatann.services.ble_data_types import BleDataStream

from flight_recorder.packets import Packet, PacketType


class SessionStates(Enum):
    IDLE = 0
    STREAMING = 1
    RECORDING = 2
    PLAYBACK = 3
    CALIBRATING = 4
    ERROR = 5


class StateUpdate(Packet):
    TYPE = PacketType.STATE_UPDATE

    @classmethod
    def decode(cls, stream: BleDataStream):
        packet_type = stream.decode(ble_data_types.Uint8)
        current_state = stream.decode(ble_data_types.Uint8)
        previous_state = stream.decode(ble_data_types.Uint8)

        if packet_type != cls.TYPE.value:
            raise Exception

        return cls(SessionStates(current_state), SessionStates(previous_state))

    def __init__(self, current_state: SessionStates, previous_state: SessionStates):
        self.current_state = current_state
        self.previous_state = previous_state

    def encode(self) -> BleDataStream:
        raise NotImplementedError
