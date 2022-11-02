import time

from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor

SESSION_TIME = 60
logger = example_utils.setup_logger(level="INFO")


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.record(flight_sensor.Rate.RATE_10_HZ, flight_sensor.Flags.ALL, False, SESSION_TIME)
    flight_sensor.disconnect()

    time.sleep(SESSION_TIME)


if __name__ == '__main__':
    main()
