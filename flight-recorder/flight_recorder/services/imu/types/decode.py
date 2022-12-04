import binascii
import logging

from blatann.services import ble_data_types
from blatann.services.ble_data_types import BleDataStream
from flight_recorder.types import Packet, PacketType
from flight_recorder.types.commands import Command
from flight_recorder.types.responses import Response
from flight_recorder.types.data import Data
from flight_recorder.types.state_update import StateUpdate


logger = logging.getLogger(__name__)


def decode_packet(stream: BleDataStream) -> Packet:
    packet_lookup = {
        PacketType.COMMAND.value: Command,
        PacketType.RESPONSE.value: Response,
        PacketType.STATE_UPDATE.value: StateUpdate,
        PacketType.DATA.value: Data,
    }
    try:
        packet_type = stream.decode(ble_data_types.Uint8)
        stream.decode_index = 0
        return packet_lookup[packet_type].decode(stream)
    except IndexError as e:
        logger.error("Failed to decode data, stream: [{}]".format(binascii.hexlify(stream.value)))
        raise(e)

