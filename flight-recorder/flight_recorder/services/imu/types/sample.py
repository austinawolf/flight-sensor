from dataclasses import dataclass
from typing import Tuple


@dataclass
class Sample:
    timestamp: int
    flags: int
    accel: Tuple[int, int, int]
    gyro: Tuple[int, int, int]
    compass: Tuple[int, int, int]
    quat: Tuple[int, int, int, int]
