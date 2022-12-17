import math
import threading
import pyvista as pv
from flight_analysis.animators import AnimatorBase, Orientation
from flight_analysis.config.flight_analysis_config import AnimatorConfig


class PyVistaAnimator(AnimatorBase):
    def __init__(self, config: AnimatorConfig):
        threading.Thread.__init__(self)
        self.config = config
        self._run = True
        self.orientation = None
        self._callback = None
        self._x = 0.0
        self._y = 0.0
        self._z = 0.0

    def _initialize(self):
        self.p = pv.Plotter()
        self.p.add_axes(interactive=True)
        axes = pv.Axes(show_actor=True, actor_scale=2.0, line_width=5)
        axes.origin = (0.0, 0.0, 0.0)
        if self.config.enable_sliders:
            self.setup_sliders()

    @staticmethod
    def load_object():
        #return pv.Cube()
        return pv.read("flight_analysis/models/assembly.STL")

    def on_x(self, value):
        self._x = float(value)
        self._update()

    def on_y(self, value):
        self._y = float(value)
        self._update()

    def on_z(self, value):
        self._z = float(value)
        self._update()

    def _update(self):
        if self._callback:
            self._callback(self._x, self._y, self._z)

    def setup_sliders(self):
        min_value = -180
        max_value = 180
        self.p.add_slider_widget(self.on_x, [min_value, max_value], title='X', pointa=(0.4, 0.95), pointb=(0.9, 0.95))
        self.p.add_slider_widget(self.on_y, [min_value, max_value], title='Y', pointa=(0.4, 0.80), pointb=(0.9, 0.80))
        self.p.add_slider_widget(self.on_z, [min_value, max_value], title='Z', pointa=(0.4, 0.65), pointb=(0.9, 0.65))

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
        self.orientation = Orientation(theta, (-x, y, -z))

    def on_event(self, callback):
        self._callback = callback

    def run(self):
        self._initialize()
        obj = self.load_object()

        while True:
            if self.orientation:
                rot = obj.copy()
                rot.rotate_vector(self.orientation.v, angle=math.degrees(self.orientation.theta))
                self.p.add_mesh(rot, name='Object', show_edges=True)
                self.orientation = None
            self.p.show(interactive_update=True)
            self.p.update()

    def kill(self):
        self._run = False
