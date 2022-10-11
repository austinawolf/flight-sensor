import os
import math
import threading
from vedo import load,  Plotter


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
        return math.cos(self.yaw) * math.cos(self.pitch),math.sin(self.yaw) * math.cos(self.pitch), math.sin(self.pitch)

    @property
    def rotation(self):
        return self.roll + self.yaw

    def __str__(self):
        return f"({self.roll}, {self.pitch}, {self.yaw})"


class Animator(threading.Thread):
    MODEL_PATH = 'flight_analysis/models/assembly.STL'

    def __init__(self):
        threading.Thread.__init__(self)
        self.plt = Plotter(axes=1, interactive=False)
        model_file = os.path.join(os.getcwd(), self.MODEL_PATH)
        self.obj = load(model_file)
        self.orientation = Orientation.from_degrees(0, 0, 0)
        if self.obj is None:
            raise FileNotFoundError(model_file)
        self._run = True

    def set_rotation(self, roll, pitch, yaw):
        self.orientation = Orientation.from_degrees(roll, pitch, yaw)

    def run(self):
        while self._run:
            #print(self.orientation)
            self.obj.orientation(self.orientation.vector, rotation=self.orientation.rotation, rad=True)
            self.plt.show(self.obj)

    def kill(self):
        self._run = False
