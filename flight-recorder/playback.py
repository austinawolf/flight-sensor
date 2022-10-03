import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor

logger = example_utils.setup_logger(level="INFO")

OUTPUT_DIR = "./recordings"


def main():
    report_name = input("Report Name: ")

    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.playback()
    flight_sensor.wait_for_idle(timeout=60)
    flight_sensor.disconnect()
    flight_sensor.imu_service.save_report(f"{OUTPUT_DIR}/{report_name}.xlsx")


if __name__ == '__main__':
    main()
