import math
import os
import time

from flight_analysis.config.flight_analysis_config import AnimatorConfig
from flight_analysis.factory.animator_factory import AnimatorFactory
from flight_analysis.fusion.madgwick.madgwick import MadgwickAHRS
from flight_analysis.session.session import FlightSession
import matplotlib.pyplot as plt


OUTPUT_DIRECTORY = "recordings"


def main():
    config = AnimatorConfig(lib="pyvista", enable_sliders=False)
    factory = AnimatorFactory(config)

    animator = factory.create_animator()
    animator.start()

    files = os.listdir(OUTPUT_DIRECTORY)
    paths = [os.path.join(OUTPUT_DIRECTORY, basename) for basename in files]
    path = max(paths, key=os.path.getctime)

    session = FlightSession.load(path)
    print(f"Found {session.name}")

    fusion = MadgwickAHRS(0.010, 0.1, 0.0)

    session.samples.sort(key=lambda s: s.timestamp)
    for sample in session.samples:
        fusion.update(sample.gyro, sample.accel, sample.compass)
        animator.set_orientation(fusion.quaternion.q)
        roll, pitch, yaw = fusion.quaternion.to_euler_angles()
        print(sample.timestamp, math.degrees(roll), math.degrees(pitch), math.degrees(yaw))
        time.sleep(fusion.samplePeriod)


main()
