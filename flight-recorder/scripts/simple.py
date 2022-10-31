import math
import time

from flight_analysis.factory.animator_factory import AnimatorConfig, AnimatorFactory
from flight_recorder.services.ble_imu.session import Quaternion


def main():
    config = AnimatorConfig(lib="vedo", model="disc")
    factory = AnimatorFactory(config)
    animator = factory.create_animator()
    animator.start()

    i = 0
    try:
        while True:
            i = i + 1 if i < 360 else 0

            roll = 45
            pitch = i
            yaw = 0

            quat = Quaternion.from_euler(math.radians(roll), math.radians(pitch), math.radians(yaw))
            quat.normalize()
            print(f"Euler: {roll, pitch, yaw},  Quat: {quat.tuple}")

            animator.set_orientation(quat.tuple)
            time.sleep(0.02)

    except Exception as e:
        print(e)
    finally:
        animator.kill()


main()
