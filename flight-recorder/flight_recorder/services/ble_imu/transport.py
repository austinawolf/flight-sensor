import binascii
import logging
from typing import List, Callable, Any
from blatann.services.ble_data_types import BleDataStream
from blatann.waitables import Waitable
from flight_recorder.packets import PacketType
from flight_recorder.packets.commands import Command
from flight_recorder.packets.data import Data
from flight_recorder.packets.decode import decode_packet
from flight_recorder.packets.responses import Response
from flight_recorder.packets.state_update import StateUpdate

logger = logging.getLogger(__name__)


class ResponseWaitable(Waitable):
    def on_response(self, response):
        self._notify(response)


class Transaction:
    def __init__(self, command, waitable: ResponseWaitable):
        self.command = command
        self.waitable = waitable


class BleImuTransportLayer:
    def __init__(self,
                 transmit: Callable[[BleDataStream], Any],
                 on_data: Callable[[Data], Any],
                 on_state_update: Callable[[StateUpdate], Any]):
        self.pending: List[Transaction] = []
        self.transmit = transmit
        self._on_data = on_data
        self._on_state_update = on_state_update

    def on_receive(self, stream):
        packet = decode_packet(stream)
        if packet.TYPE == PacketType.RESPONSE:
            self._on_response(
                packet)
        elif packet.TYPE == PacketType.STATE_UPDATE:
            self._on_state_update(packet)
        elif packet.TYPE == PacketType.DATA:
            self._on_data(packet)
        else:
            pass

    def _on_response(self, response: Response):
        logger.info(f"New Response: [{response.opcode}]")
        for transaction in self.pending:
            if self.is_response(transaction.command, response):
                self.pending.remove(transaction)
                transaction.waitable.on_response(response)
                return

    def _add_to_pending(self, transaction):
        self.pending.append(transaction)

    def send_command(self, command: Command):
        waitable = ResponseWaitable()
        transaction = Transaction(command, waitable)
        self.transmit(command.encode())
        self._add_to_pending(transaction)
        return waitable

    @staticmethod
    def is_response(command: Command, response: Response):
        return command.opcode.value == response.opcode.value
