import logging
from enum import Enum

from blatann import BleDevice
from blatann.gap.gap_types import ConnectionParameters
from blatann.nrf import nrf_events

from flight_recorder.client import Client
from flight_recorder.services.ble_imu import BleImuService

logger = logging.getLogger(__name__)


class FlightSensor:
    DEVICE_NAME = "Flight Sensor"

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

    class ConnectionParams:
        def __init__(self, name):
            self.name = name

    def __init__(self, port, timeout=10):
        self.port = port
        self.timeout = timeout
        self.ble_device = BleDevice(self.port)
        self.imu_service = BleImuService()
        self._peer = None

    def connect(self, params: ConnectionParams = None):
        if not params:
            params = self.ConnectionParams(self.DEVICE_NAME)

        # setup ble device
        self.ble_device.event_logger.suppress(nrf_events.GapEvtAdvReport)
        self.ble_device.configure()
        self.ble_device.open()
        self.ble_device.scanner.set_default_scan_params(timeout_seconds=10)

        logger.info("Scanning for '{}'".format(params.name))

        target_address = None
        for report in self.ble_device.scanner.start_scan().scan_reports:
            if report.advertise_data.local_name == params.name:
                target_address = report.peer_address
                break

        if not target_address:
            logger.info("Did not find target peripheral")
            raise Exception

        # Initiate the connection and wait for it to finish
        cp = ConnectionParameters(7.5, 7.5, 4000, 0)

        logger.info("Found match: connecting to address {}".format(target_address))
        waitable = self.ble_device.connect(target_address, connection_params=cp)
        self.ble_device.connecting_peripheral.preferred_mtu_size = self.ble_device.max_mtu_size
        self._peer = waitable.wait()

        if not self._peer:
            logger.warning("Timed out connecting to device")
            return
        logger.info("Connected, conn_handle: {}".format(self._peer.conn_handle))

        logger.info("Update MTU")
        self._peer.exchange_mtu(self._peer.max_mtu_size).wait()

        logger.info(f"Connection Interval: {self._peer.active_connection_params.interval_ms}")

        # Wait up to 10 seconds for service discovery to complete
        _, event_args = self._peer.discover_services().wait(10, exception_on_timeout=False)
        logger.info("Service discovery complete! status: {}".format(event_args.status))

        # Initializes services
        self.imu_service.initialize(self._peer)

    def disconnect(self):
        logger.info("Disconnecting from peripheral")
        if self._peer:
            self._peer.disconnect().wait()

    def stream(self, rate: Rate, flags: int, time: int):
        response = self.imu_service.stream(rate.value, flags, time)
        if response.is_error:
            raise Exception(response)

    def record(self, rate: Rate, flags: int, stream_enable: bool, time: int):
        response = self.imu_service.record(rate.value, flags, stream_enable, time)
        if response.is_error:
            raise Exception(response)

    def playback(self):
        response = self.imu_service.playback()
        if response.is_error:
            raise Exception(response)

    def stop(self):
        response = self.imu_service.stop()
        if response.is_error:
            raise Exception(response)

    def calibrate(self):
        response = self.imu_service.calibrate()
        if response.is_error:
            raise Exception(response)
