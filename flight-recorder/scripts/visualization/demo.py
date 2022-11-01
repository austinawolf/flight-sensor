import math

from flight_analysis.animators import AnimatorBase
from flight_analysis.factory.animator_factory import AnimatorConfig, AnimatorFactory
from flight_recorder.services.ble_imu.session import Quaternion

animator: AnimatorBase = None


def on_event(roll, pitch, yaw):
    global animator
    quat = Quaternion.from_euler(math.radians(roll), math.radians(pitch), math.radians(yaw))
    quat.normalize()
    print(f"Euler: {roll, pitch, yaw},  Quat: {quat.tuple}")
    animator.set_orientation(quat.tuple)


def main():
    global animator
    config = AnimatorConfig(lib="pyvista")
    factory = AnimatorFactory(config)
    animator = factory.create_animator()
    animator.on_event(on_event)
    animator.start()

    while True:
        pass


main()
