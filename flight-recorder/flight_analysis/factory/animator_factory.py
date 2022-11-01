from flight_analysis.animators.pyvista import PyVistaAnimator
from flight_analysis.animators.vedo import VedoAnimator
from flight_analysis.config.flight_analysis_config import AnimatorConfig


class AnimatorFactory:
    def __init__(self, config: AnimatorConfig):
        self.config = config

    def create_animator(self):
        if self.config.lib == "vedo":
            return VedoAnimator()
        elif self.config.lib == "pyvista":
            return PyVistaAnimator(self.config)
        else:
            raise ValueError(self.config.lib)
