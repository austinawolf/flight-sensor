import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor

logger = example_utils.setup_logger(level="INFO")

SESSION_TIME = 10


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.initialize()
    flight_sensor.record(flight_sensor.Rate.RATE_100_HZ, flight_sensor.Flags.ALL, False, SESSION_TIME)
    flight_sensor.deinitialize()


if __name__ == '__main__':
    main()
