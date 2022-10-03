import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor

logger = example_utils.setup_logger(level="INFO")

SESSION_TIME = 10
OUTPUT_DIR = "./streams"


def main():
    report_name = input("Filename: ")

    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.stream(flight_sensor.Rate.RATE_1_HZ, flight_sensor.Flags.ALL, SESSION_TIME)
    time.sleep(SESSION_TIME)
    flight_sensor.wait_for_idle()
    flight_sensor.disconnect()
    flight_sensor.imu_service.save_report(f"{OUTPUT_DIR}/{report_name}.xlsx")


if __name__ == '__main__':
    main()
