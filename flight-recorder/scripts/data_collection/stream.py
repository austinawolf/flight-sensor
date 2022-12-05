import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor


logger = example_utils.setup_logger(level="INFO")

SESSION_TIME = 10
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

    imu_service.stream(flight_sensor.Rate.RATE_100_HZ, flight_sensor.Flags.ALL, SESSION_TIME)

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

    print(f"Collected {len(samples)} samples")


if __name__ == '__main__':
    main()
