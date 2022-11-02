import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor
logger = example_utils.setup_logger(level="INFO")


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.calibrate()
    flight_sensor.wait_for_idle(timeout=30)

    flight_sensor.disconnect()


if __name__ == '__main__':
    main()