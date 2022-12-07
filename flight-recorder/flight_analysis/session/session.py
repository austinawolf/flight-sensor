import logging
from datetime import datetime
from typing import List

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
