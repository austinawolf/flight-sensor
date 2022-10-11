from flight_analysis.animator import Animator
from blatann.examples import example_utils
from flight_recorder.flight_sensor import FlightSensor
from flight_recorder.packets.data import Data
from flight_recorder.services.ble_imu.session import Quaternion


logger = example_utils.setup_logger(level="WARNING")
animator = Animator()


def on_data(data: Data):
    if data is None:
        return
    quat = Quaternion(data.quat)
    quat.normalize()
    animator.set_rotation(quat.yaw, -quat.pitch, -quat.roll)
    print(quat.roll, quat.pitch, quat.yaw)


def main():
    animator.start()

    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.imu_service.data_stream.subscribe(on_data)

    flight_sensor.stream(flight_sensor.Rate.RATE_10_HZ, flight_sensor.Flags.ALL)

    try:
        while True:
            pass
    except Exception:
        flight_sensor.disconnect()


if __name__ == '__main__':
    main()
