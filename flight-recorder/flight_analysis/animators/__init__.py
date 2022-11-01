import threading
from dataclasses import dataclass
from typing import Tuple


@dataclass
class Orientation:
    theta: float
    v: Tuple[float, float, float]


class AnimatorBase(threading.Thread):
    def set_orientation(self, quat):
        raise NotImplementedError

    def run(self):
        raise NotImplementedError

    def on_event(self, callback):
        raise NotImplementedError
