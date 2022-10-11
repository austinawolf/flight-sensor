from flight_analysis.animators.pyvista import PyVistaAnimator


def main():
    animator = PyVistaAnimator()
    animator.start()

    while True:
        roll = float(input("Roll: "))
        pitch = float(input("Pitch: "))
        yaw = float(input("Yaw: "))
        animator.set_rotation(roll, pitch, yaw)


main()
