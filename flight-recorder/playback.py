import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor
from flight_recorder.packets import SessionStates

logger = example_utils.setup_logger(level="INFO")


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.initialize()

    flight_sensor.playback()

    while flight_sensor.client.current_state == SessionStates.IDLE:
        pass

    while flight_sensor.client.current_state == SessionStates.PLAYBACK:
        pass

    flight_sensor.deinitialize()


if __name__ == '__main__':
    main()
