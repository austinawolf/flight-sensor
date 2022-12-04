import logging
from typing import Callable

from blatann.waitables import Waitable

logger = logging.getLogger(__name__)


class ResponseWaitable(Waitable):
    def on_response(self, response):
        self._notify(response)


class TransportLayerBase:
    def send_command(self, command_payload: bytes, payload_handler: Callable[[bytes], None]):
        raise NotImplementedError

    @property
    def on_update(self):
        raise NotImplementedError

