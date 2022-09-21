import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor
from flight_recorder.packets import SessionStates

logger = example_utils.setup_logger(level="INFO")


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.calibrate()

    while flight_sensor.imu_service.current_state == SessionStates.IDLE:
        pass

    while flight_sensor.imu_service.current_state == SessionStates.CALIBRATING:
        pass

    flight_sensor.disconnect()


if __name__ == '__main__':
    main()
