import threading
import pyvista as pv
from flight_analysis.animators import AnimatorBase, Orientation


class PyVistaAnimator(AnimatorBase):
    OBJ = pv.read("flight_analysis/models/assembly.STL")
    MODEL_PATH = 'flight_analysis/animators/assembly.STL'

    def __init__(self):
        threading.Thread.__init__(self)
        self.orientation = Orientation.from_degrees(0, 0, 0)
        self._run = True
        self.orientation = Orientation(0, 0, 0)

    def _initialize(self):
        self.p = pv.Plotter()
        self.p.add_axes(interactive=True)

        axes = pv.Axes(show_actor=True, actor_scale=2.0, line_width=5)
        axes.origin = (0.0, 0.0, 0.0)
        self.set_rotation(0, 0, 0)

    def load_object(self):
        #return self.OBJ
        return pv.Cube()

    def set_rotation(self, roll, pitch, yaw):
        self.orientation = Orientation(roll, pitch, yaw)

    def run(self):
        self._initialize()
        while True:
            if self.orientation:
                obj = self.load_object()
                obj = obj.rotate_x(self.orientation.roll, inplace=False)
                obj = obj.rotate_y(self.orientation.pitch, inplace=False)
                obj = obj.rotate_z(self.orientation.yaw, inplace=False)
                self.p.add_mesh(obj, name='Object', show_edges=True)
            self.p.show(interactive_update=True)
            self.p.update()

    def kill(self):
        self._run = False
