import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor
from flight_recorder.packets import SessionStates

logger = example_utils.setup_logger(level="INFO")

SESSION_TIME = 10


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()

    flight_sensor.stream(flight_sensor.Rate.RATE_10_HZ, flight_sensor.Flags.ALL, SESSION_TIME)
    while flight_sensor.imu_service.current_state.value == SessionStates.IDLE.value:
        pass

    while flight_sensor.imu_service.current_state.value == SessionStates.STREAMING.value:
        pass

    flight_sensor.disconnect()


if __name__ == '__main__':
    main()
