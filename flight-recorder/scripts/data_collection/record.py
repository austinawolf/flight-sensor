import time

from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor

SESSION_TIME = 15
logger = example_utils.setup_logger(level="INFO")


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()

    imu_service = flight_sensor.imu_service

    imu_service.record(flight_sensor.Rate.RATE_1_HZ, flight_sensor.Flags.ALL, False, SESSION_TIME)
    flight_sensor.disconnect()

    time.sleep(SESSION_TIME)


if __name__ == '__main__':
    main()
