import logging
from blatann import BleDevice
from flight_recorder.client import Client

logger = logging.getLogger(__name__)


class FlightSensor:
    def __init__(self, port, timeout=10):
        self.port = port
        self.timeout = timeout
        self.ble_device = BleDevice(self.port)
        self.client = Client(self.ble_device)

    def initialize(self):
        self.client.initialize()

    def deintialize(self):
        self.client.deinitialize()
