import time
from flight_analysis.animator import Animator


def main():
    animator = Animator()
    animator.start()

    while True:
        roll = int(input("Roll: "))
        pitch = int(input("Pitch: "))
        yaw = int(input("Yaw: "))
        animator.set_rotation(yaw, -pitch, -roll)


main()
