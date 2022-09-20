import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor

logger = example_utils.setup_logger(level="INFO")


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.initialize()
    flight_sensor.start_playback()
    time.sleep(120)
    #flight_sensor.stop_playback()
    flight_sensor.deinitialize()


if __name__ == '__main__':
    main()
