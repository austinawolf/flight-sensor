import math
import threading
import pyvista as pv
from flight_analysis.animators import AnimatorBase, Orientation


class PyVistaAnimator(AnimatorBase):
    def __init__(self):
        threading.Thread.__init__(self)
        self._run = True
        self.orientation = None

    def _initialize(self):
        self.p = pv.Plotter()
        self.p.add_axes(interactive=True)
        axes = pv.Axes(show_actor=True, actor_scale=2.0, line_width=5)
        axes.origin = (0.0, 0.0, 0.0)

    def load_object(self):
        #return pv.Cube()
        return pv.read("flight_analysis/models/assembly.STL")

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
        raise NotImplementedError

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
