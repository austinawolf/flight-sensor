import threading
from enum import Enum


class AnimatorBase(threading.Thread):
    def set_orientation(self, quat):
        raise NotImplementedError

    def run(self):
        raise NotImplementedError

    def on_event(self, callback):
        raise NotImplementedError
