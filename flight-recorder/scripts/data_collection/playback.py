import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor

logger = example_utils.setup_logger(level="INFO")

OUTPUT_DIR = "../../recordings"


def main():
    samples = []

    def on_sample(new_sample):
        if not new_sample:
            return
        samples.append(new_sample)

    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()

    imu_service = flight_sensor.imu_service
    imu_service.on_sample.subscribe(on_sample)

    imu_service.playback()
    imu_service.wait_for_idle(timeout=60)
    flight_sensor.disconnect()

    print(f"Collected {len(samples)} samples")


if __name__ == '__main__':
    main()
