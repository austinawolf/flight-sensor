import time
from flight_analysis.animators.pyvista import PyVistaAnimator


def main():
    animator = PyVistaAnimator()
    animator.start()

    i = 0
    try:
        while True:
            roll = 0
            pitch = 0
            yaw = i
            animator.set_rotation(roll, pitch, yaw)
            i = i + 1 if i < 360 else 0
            time.sleep(0.001)
    except Exception:
        pass
    finally:
        animator.kill()


main()
