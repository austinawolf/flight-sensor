import time
from dataclasses import dataclass
from typing import Tuple

from flight_analysis.factory.animator_factory import AnimatorConfig, AnimatorFactory
from flight_recorder.services.imu.session import Quaternion


@dataclass
class Frame:
    timestamp: int
    quat: Tuple[float, float, float, float]


def main():
    file = open("./recordings/throw2.csv", "r")
    lines = file.readlines()[1:]

    config = AnimatorConfig(lib="pyvista")
    factory = AnimatorFactory(config)
    animator = factory.create_animator()
    animator.start()

    frames = []
    for line in lines:
        timestamp, flags, index, \
        gx, gy, gz, \
        ax, ay, az, \
        cx, cy, cz, \
        q0, q1, q2, q3, \
        roll, pitch, yaw = line.split(",")

        quat = Quaternion((float(q0), float(q1), float(q2), float(q3)))
        quat.normalize()

        frames.append(Frame(int(timestamp), (float(quat.q0), float(quat.q1), float(quat.q2), float(quat.q3))))

    start_index = 913
    stop_index = 1352

    for frame in frames[start_index: stop_index]:
        animator.set_orientation(frame.quat)
        time.sleep(0.10)

    animator.kill()


main()
