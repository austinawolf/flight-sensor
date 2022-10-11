import time
from flight_analysis.animator import Animator


def main():
    animator = Animator()
    animator.start()
    #
    # while True:
    #     roll = int(input("Roll: "))
    #     pitch = int(input("Pitch: "))
    #     yaw = int(input("Yaw: "))
    #     animator.set_rotation(yaw, -pitch, -roll)

    i = 0
    try:
        while True:
            roll = i
            pitch = i
            yaw = i
            animator.set_rotation(yaw, -pitch, -roll)
            i = i + 1 if i < 360 else 0
            time.sleep(0.01)
    except Exception:
        pass
    finally:
        animator.kill()


main()
