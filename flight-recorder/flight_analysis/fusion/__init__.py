from flight_analysis.session.quaternion import Quaternion
from flight_analysis.session.sample import Sample


class AbstractSensorFusion:
    @property
    def name(self) -> str:
        raise NotImplementedError

    def update(self, sample: Sample) -> Quaternion:
        raise NotImplementedError


