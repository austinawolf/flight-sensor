import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor


logger = example_utils.setup_logger(level="INFO")

SESSION_TIME = 0
OUTPUT_DIR = "../../recordings"


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    imu_service = flight_sensor.imu_service

    response = imu_service.stream(flight_sensor.Rate.RATE_10_HZ, flight_sensor.Flags.ALL, SESSION_TIME)
    logger.info(f"Status: {response.status}")

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


if __name__ == '__main__':
    main()
