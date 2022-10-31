import os.path

from vedo import load, Arrow

from flight_analysis.animators.vedo import VedoAnimator


class AnimatorConfig:
    def __init__(self, lib="vedo", model="disc"):
        self.lib = lib
        self.model = model


class AnimatorFactory:
    def __init__(self, config: AnimatorConfig):
        self.config = config

    def create_model(self):
        if self.config.lib == "vedo":
            if self.config.model == "disc":
                return load('flight_analysis/models/assembly.STL').c("red")
            elif self.config.model == "arrow":
                return Arrow()
            else:
                raise ValueError(self.config.model)
        else:
            raise ValueError(self.config.lib)

    def create_animator(self):
        model = self.create_model()
        if self.config.lib == "vedo":
            return VedoAnimator(model)
        else:
            raise ValueError(self.config.lib)
