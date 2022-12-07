from typing import Tuple

from flight_analysis.session.quaternion import Quaternion
from flight_analysis.util.serializable import Serializable


class Sample(Serializable):
    timestamp: int
    flags: int
    accel: Tuple[float, float, float]
    gyro: Tuple[float, float, float]
    compass: Tuple[float, float, float]
    quat: Quaternion

    def __init__(self, timestamp, flags, accel, gyro, compass, quat):
        self.timestamp = timestamp
        self.flags = flags
        self.accel = accel
        self.gyro = gyro
        self.compass = compass
        self.quat = quat
