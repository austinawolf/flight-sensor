import time
from dataclasses import dataclass

from flight_analysis.animator import Animator


@dataclass
class Frame:
    timestamp: int
    roll: float
    pitch: float
    yaw: float


def main():
    file = open("throw2.csv", "r")
    lines = file.readlines()[1:]

    animator = Animator()
    animator.start()

    frames = []
    for line in lines:
        timestamp, flags, index, \
        gx, gy, gz, \
        ax, ay, az, \
        cx, cy, cz, \
        q0, q1, q2, q3, \
        roll, pitch, yaw = line.split(",")
        frames.append(Frame(int(timestamp), float(roll), float(pitch), float(yaw)))

    start_index = 913
    stop_index = 1352

    for frame in frames[start_index: stop_index]:
        animator.set_rotation(frame.yaw, -frame.pitch, -frame.roll)
        time.sleep(0.10)

    animator.kill()


main()
