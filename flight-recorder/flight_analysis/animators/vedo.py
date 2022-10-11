import threading
from vedo import load, Plotter
from flight_analysis.animators import AnimatorBase, Orientation


class VedoAnimator(AnimatorBase):
    MODEL_PATH = 'flight_analysis/animators/assembly.STL'

    def __init__(self):
        threading.Thread.__init__(self)
        self.plt = Plotter(axes=1, interactive=False)
        self.obj = self.load_object()
        self.orientation = Orientation.from_degrees(0, 0, 0)
        if self.obj is None:
            raise FileNotFoundError()
        self._run = True

    def load_object(self):
        return load(self.MODEL_PATH)

    def set_rotation(self, roll, pitch, yaw):
        self.orientation = Orientation.from_degrees(roll, pitch, yaw)

        self.obj.orientation(self.orientation.vector, rotation=0, rad=True)
        self.orientation = None

    def run(self):
        while self._run:
            if self.orientation is not None:

            self.plt.show(self.obj)

    def kill(self):
        self._run = False
