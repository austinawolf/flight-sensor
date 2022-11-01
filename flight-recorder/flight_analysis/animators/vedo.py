import copy
import math
import threading
from dataclasses import dataclass
from typing import Tuple

from vedo import load, Plotter, Mesh, dataurl, Arrow
from flight_analysis.animators import AnimatorBase, Orientation


class VedoAnimator(AnimatorBase):
    def __init__(self, enable_sliders):
        threading.Thread.__init__(self)
        self.plt = Plotter(axes=1, interactive=False)
        self.obj = self.load_object()
        self.orientation = None
        self._callback = None
        self._x = 0.0
        self._y = 0.0
        self._z = 0.0
        if enable_sliders:
            self.setup_sliders()
        self._run = True

    @staticmethod
    def load_object():
        obj = load('flight_analysis/models/assembly.STL').c("red")
        if obj is None:
            raise RuntimeError
        return obj

    def on_x(self, widget, event):
        self._x = widget.GetRepresentation().GetValue()
        self._update()

    def on_y(self, widget, event):
        self._y = widget.GetRepresentation().GetValue()
        self._update()

    def on_z(self, widget, event):
        self._z = widget.GetRepresentation().GetValue()
        self._update()

    def _update(self):
        if self._callback:
            self._callback(self._x, self._y, self._z)

    def setup_sliders(self):
        min_value = -180
        max_value = 180
        value = 0
        self.plt.addSlider2D(self.on_x, min_value, max_value, value=value, title="x", pos=2)
        self.plt.addSlider2D(self.on_y, min_value, max_value, value=value, title="y", pos=3)
        self.plt.addSlider2D(self.on_z, min_value, max_value, value=value, title="z", pos=4)

    def set_orientation(self, quat):
        theta = math.acos(quat[0]) * 2.0
        if math.sin(theta / 1.0) == 0.0:
            x = 0.0
            y = 0.0
            z = 0.0
        else:
            x = quat[1] / math.sqrt(1 - quat[0] * quat[0])
            y = quat[2] / math.sqrt(1 - quat[0] * quat[0])
            z = quat[3] / math.sqrt(1 - quat[0] * quat[0])
        print(f"Axis-Angle: {theta, x, y, z}")
        self.orientation = Orientation(theta, (x, y, z))

    def on_event(self, callback):
        self._callback = callback

    def run(self):
        while self._run:
            if self.orientation is not None:
                self.obj.orientation(self.orientation.v, rotation=self.orientation.theta, rad=True)
                self.orientation = None
            self.plt.show(self.obj)

    def kill(self):
        self._run = False
