import os

from flight_analysis.fusion.dmp import DmpFusion
from flight_analysis.fusion.madgwick import Madgwick
from flight_analysis.session.session import FlightSession


OUTPUT_DIRECTORY = "recordings"


def main():
    files = os.listdir(OUTPUT_DIRECTORY)
    paths = [os.path.join(OUTPUT_DIRECTORY, basename) for basename in files]
    path = max(paths, key=os.path.getctime)

    session = FlightSession.load(path)
    print(f"Found {session.name}")

    fusions = [
        DmpFusion(),
        Madgwick(0.010, 0.2, 0.0, imu_only=False),
        Madgwick(0.010, 0.5, 0.0, imu_only=False),
        Madgwick(0.010, 0.7, 0.0, imu_only=False)
    ]

    for i, fusion in enumerate(fusions):
        session.run_sensor_fusion(fusion)
        session.plot_euler(fusion.name, block=(i == (len(fusions) - 1)))


main()
