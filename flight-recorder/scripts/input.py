import time
from flight_analysis.animator import Animator


def main():
    animator = Animator()
    animator.start()

    while True:
        roll = float(input("Roll: "))
        pitch = float(input("Pitch: "))
        yaw = float(input("Yaw: "))
        animator.set_rotation(yaw, -pitch, -roll)


main()
