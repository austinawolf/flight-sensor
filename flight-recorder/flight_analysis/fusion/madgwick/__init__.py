from flight_analysis.fusion import AbstractSensorFusion
from flight_analysis.fusion.madgwick.madgwick import MadgwickAHRS
from flight_analysis.session.quaternion import Quaternion


class Madgwick(AbstractSensorFusion):
    def __init__(self, sample_period, beta, zeta, imu_only=False):
        self.sample_period = sample_period
        self.madgwick_ahrs = MadgwickAHRS(sample_period, beta, zeta)
        self.imu_only = imu_only

    @property
    def name(self):
        return f"Madgwick-{self.madgwick_ahrs.beta}-{self.madgwick_ahrs.zeta}"

    def update(self, sample):
        if self.imu_only:
            self.madgwick_ahrs.update_imu(sample.gyro_rps, sample.accel_gs)
        else:
            self.madgwick_ahrs.update(sample.gyro_rps, sample.accel_gs, sample.compass_uts)
        return Quaternion.from_tuple(self.madgwick_ahrs.quaternion.q)

