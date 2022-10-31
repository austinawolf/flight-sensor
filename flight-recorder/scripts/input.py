import time
from flight_analysis.animators.vedo import VedoAnimator


def main():
    animator = VedoAnimator()
    animator.start()

    while True:
        q0 = float(input("q0: "))
        q1 = float(input("q1: "))
        q2 = float(input("q2: "))
        q3 = float(input("q3: "))
        animator.set_orientation((q0, q1, q2, q3))
        time.sleep(1.0)


main()
