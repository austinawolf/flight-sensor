import os
import time
from datetime import datetime
from typing import List

from blatann.examples import example_utils
from flight_analysis.session.quaternion import Quaternion
from flight_analysis.session.sample import Sample
from flight_analysis.session.session import FlightSession
from flight_recorder.flight_sensor import FlightSensor

logger = example_utils.setup_logger(level="INFO")

OUTPUT_DIR = "recordings"


def main():
    samples: List[Sample] = []

    def on_sample(new_sample):
        if not new_sample:
            return

        quaternion = Quaternion(new_sample.quat[0], new_sample.quat[1], new_sample.quat[2], new_sample.quat[3])
        sample = Sample(new_sample.timestamp, new_sample.flags,
                        new_sample.accel, new_sample.gyro,
                        new_sample.compass, quaternion)

        samples.append(sample)

    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()

    imu_service = flight_sensor.imu_service
    imu_service.on_sample.subscribe(on_sample)

    imu_service.playback()
    imu_service.wait_for_idle(timeout=60)
    flight_sensor.disconnect()

    session = FlightSession("1234", datetime.now(), 2, 3, False, samples)

    path = os.path.join(OUTPUT_DIR, f"{session.name}.json")
    session.save(path)

    logger.info(f"Saved {len(samples)} samples to {path}")


if __name__ == '__main__':
    main()
