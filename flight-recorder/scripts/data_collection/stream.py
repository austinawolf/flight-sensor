import os
import time
from typing import List
from datetime import datetime
from blatann.examples import example_utils

from flight_analysis.session.quaternion import Quaternion
from flight_analysis.session.sample import Sample
from flight_analysis.session.session import FlightSession
from flight_recorder.flight_sensor import FlightSensor

logger = example_utils.setup_logger(level="INFO")

SESSION_TIME = 10
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

    imu_service.stream(flight_sensor.Rate.RATE_10_HZ, flight_sensor.Flags.ALL, SESSION_TIME)

    try:
        if SESSION_TIME:
            time.sleep(SESSION_TIME)
        else:
            while True:
                pass
    except KeyboardInterrupt:
        imu_service.stop()

    imu_service.wait_for_idle()

    flight_sensor.disconnect()

    session = FlightSession("1234", datetime.now(), 2, 3, True, samples)

    path = os.path.join(OUTPUT_DIR, f"{session.name}.json")
    session.save(path)

    logger.info(f"Saved {len(samples)} samples to {path}")


if __name__ == '__main__':
    main()
