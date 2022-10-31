from flight_analysis.animators.pyvista import PyVistaAnimator
from flight_analysis.animators.vedo import VedoAnimator


class AnimatorConfig:
    def __init__(self, lib="vedo"):
        self.lib = lib


class AnimatorFactory:
    def __init__(self, config: AnimatorConfig):
        self.config = config

    def create_animator(self):
        if self.config.lib == "vedo":
            return VedoAnimator()
        elif self.config.lib == "pyvista":
            return PyVistaAnimator()
        else:
            raise ValueError(self.config.lib)
