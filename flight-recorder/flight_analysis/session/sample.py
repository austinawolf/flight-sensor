import math
from typing import Tuple

from flight_analysis.session.quaternion import Quaternion
from flight_analysis.util.fixed_point import fixed_to_float
from flight_analysis.util.serializable import Serializable


class Sample(Serializable):
    ACCEL_FSR_GS = 2
    GYRO_FSR_DPS = 250
    COMPASS_FSR_UT = 4900

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

    @property
    def gyro_rps(self):
        return [math.radians(fixed_to_float(g, 16, self.GYRO_FSR_DPS)) for g in self.gyro]

    @property
    def accel_gs(self):
        return [fixed_to_float(a, 16, self.ACCEL_FSR_GS) for a in self.accel]

    @property
    def compass_uts(self):
        return [fixed_to_float(c, 16, self.COMPASS_FSR_UT) for c in self.compass]
