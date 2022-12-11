import logging
from datetime import datetime
from typing import List
import matplotlib.pyplot as plt
from flight_analysis.fusion import AbstractSensorFusion
from flight_analysis.session.sample import Sample
from flight_analysis.util.serializable import Serializable

logger = logging.getLogger(__name__)


class FlightSession(Serializable):
    mac_addr: str
    timestamp: datetime
    session_time: int
    sampling_rate: int
    is_stream: bool
    samples: List[Sample]

    def __init__(self, mac_addr, timestamp, session_time, sampling_rate, is_stream, samples):
        self.mac_addr = mac_addr
        self.timestamp = timestamp
        self.session_time = session_time
        self.sampling_rate = sampling_rate
        self.is_stream = is_stream
        self.samples = samples

    @property
    def name(self):
        return f"flight_{'stream' if self.is_stream else 'playback'}" \
               f"-{self.mac_addr}" \
               f"-{self.timestamp.strftime('%m_%d_%Y-%H_%M_%S')}"

    def run_sensor_fusion(self, fusion: AbstractSensorFusion):
        self.samples.sort(key=lambda s: s.timestamp)
        for sample in self.samples:
            sample.quat = fusion.update(sample)

    def plot_euler(self, name: str, block=True):
        timestamps = [sample.timestamp / 1000 for sample in self.samples]
        roll = [sample.quat.roll for sample in self.samples]
        pitch = [sample.quat.pitch for sample in self.samples]
        yaw = [sample.quat.yaw for sample in self.samples]

        plt.figure(name)
        plt.plot(range(len(roll)), roll, label="roll")
        plt.plot(range(len(pitch)), pitch, label="pitch")
        plt.plot(range(len(yaw)), yaw, label="yaw")
        plt.legend()
        plt.show(block=block)
