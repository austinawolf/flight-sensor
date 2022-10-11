import os
import math
import threading
from vedo import load, Plotter, Cube


class Orientation:
    @classmethod
    def from_degrees(cls, roll, pitch, yaw):
        return cls(math.radians(roll), math.radians(pitch), math.radians(yaw))

    def __init__(self, roll, pitch, yaw):
        self.roll = roll
        self.pitch = pitch
        self.yaw = yaw

    @property
    def vector(self):
        return math.cos(self.yaw) * math.cos(self.pitch), math.sin(self.yaw) * math.cos(self.pitch), math.sin(self.pitch)

    @property
    def rotation(self):
        return self.roll + self.yaw

    def __str__(self):
        return f"({self.roll}, {self.pitch}, {self.yaw})"


class AnimatorBase(threading.Thread):
    def set_rotation(self, roll, pitch, yaw):
        raise NotImplementedError

    def run(self):
        raise NotImplementedError
