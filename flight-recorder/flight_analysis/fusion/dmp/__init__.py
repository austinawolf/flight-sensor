from flight_analysis.fusion import AbstractSensorFusion


class DmpFusion(AbstractSensorFusion):
    @property
    def name(self):
        return "dmp"

    def update(self, sample):
        sample.quat.normalize()
        return sample.quat
