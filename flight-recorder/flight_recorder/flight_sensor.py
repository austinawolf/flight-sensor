import logging
from enum import Enum

from blatann import BleDevice
from blatann.nrf import nrf_events

from flight_recorder.client import Client

logger = logging.getLogger(__name__)


class FlightSensor:
    class Rate(Enum):
        RATE_1_HZ = 0
        RATE_5_HZ = 1
        RATE_10_HZ = 2
        RATE_20_HZ = 3
        RATE_50_HZ = 4
        RATE_100_HZ = 5

    class Flags:
        GYRO = (1 << 0)
        ACCEL = (1 << 1)
        COMPASS = (1 << 2)
        QUAT = (1 << 3)
        IMU = GYRO | ACCEL
        ALL = IMU | COMPASS | QUAT

    class Destination(Enum):
        CENTRAL = 0
        MEMORY = 1
        BOTH = 2

    def __init__(self, port, timeout=10):
        self.port = port
        self.timeout = timeout
        self.ble_device = BleDevice(self.port)
        self.client = Client(self.ble_device)

    def initialize(self):
        # setup ble device
        self.ble_device.event_logger.suppress(nrf_events.GapEvtAdvReport)
        self.ble_device.configure()
        self.ble_device.open()
        self.ble_device.scanner.set_default_scan_params(timeout_seconds=10)

        # initialize flight sensor client
        self.client.initialize()

    def deinitialize(self):
        self.client.deinitialize()

    def stream(self, rate: Rate, flags: int, time: int):
        response = self.client.stream(rate.value, flags, time)
        if response.is_error:
            raise Exception(response)

    def record(self, rate: Rate, flags: int, stream_enable: bool, time: int):
        response = self.client.record(rate.value, flags, stream_enable, time)
        if response.is_error:
            raise Exception(response)

    def playback(self):
        response = self.client.playback()
        if response.is_error:
            raise Exception(response)

    def stop(self):
        response = self.client.stop()
        if response.is_error:
            raise Exception(response)

    def calibrate(self):
        response = self.client.calibrate()
        if response.is_error:
            raise Exception(response)
