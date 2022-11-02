import time
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor
from flight_recorder.packets.data import Data
from flight_recorder.services.ble_imu.session import Quaternion

logger = example_utils.setup_logger(level="INFO")

SESSION_TIME = 0
OUTPUT_DIR = "../../recordings"


def on_data(data: Data):
    if data is None:
        return

    quaternion = Quaternion(data.quat)
    quaternion.normalize()

    print(f"Euler: {quaternion.roll, quaternion.pitch, quaternion.yaw}")


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.imu_service.data_stream.subscribe(on_data)
    flight_sensor.stream(flight_sensor.Rate.RATE_10_HZ, flight_sensor.Flags.ALL, SESSION_TIME)

    try:
        if SESSION_TIME:
            time.sleep(SESSION_TIME)
        else:
            while True:
                pass
    except KeyboardInterrupt:
        flight_sensor.stop()

    flight_sensor.wait_for_idle()
    flight_sensor.disconnect()
    flight_sensor.imu_service.save_report(OUTPUT_DIR)


if __name__ == '__main__':
    main()
