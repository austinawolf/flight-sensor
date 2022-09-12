import time
from blatann.examples import example_utils
from flight_recorder.flight_recorder import FlightRecorder

logger = example_utils.setup_logger(level="INFO")


def main(serial_port):
    flight_recorder = FlightRecorder(serial_port, timeout=10)
    flight_recorder.initialize()
    time.sleep(5.0)
    flight_recorder.deinitialize()


if __name__ == '__main__':
    main("COM17")
