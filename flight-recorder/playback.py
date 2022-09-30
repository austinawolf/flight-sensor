import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor
from flight_recorder.packets import SessionStates

logger = example_utils.setup_logger(level="INFO")

OUTPUT_DIR = "./recordings"


def main():
    report_name = input("Report Name: ")

    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()

    flight_sensor.playback()

    while flight_sensor.imu_service.current_state == SessionStates.PLAYBACK:
        pass

    flight_sensor.disconnect()

    flight_sensor.imu_service.save_report(f"{OUTPUT_DIR}/{report_name}.xlsx")


if __name__ == '__main__':
    main()
