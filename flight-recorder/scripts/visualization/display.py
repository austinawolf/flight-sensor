from flight_analysis.animators import AnimatorBase
from flight_analysis.animators.pyvista import PyVistaAnimator
from blatann.examples import example_utils

from flight_analysis.factory.animator_factory import AnimatorConfig, AnimatorFactory
from flight_recorder.flight_sensor import FlightSensor
from flight_recorder.packets.data import Data
from flight_recorder.services.ble_imu.session import Quaternion


logger = example_utils.setup_logger(level="INFO")
animator: AnimatorBase = None


def on_data(data: Data):
    global animator

    if data is None:
        return
    quat = Quaternion(data.quat)
    quat.normalize()
    animator.set_orientation(quat.tuple)


def main():
    global animator
    config = AnimatorConfig(lib="pyvista", enable_sliders=False)
    factory = AnimatorFactory(config)
    animator = factory.create_animator()
    animator.start()

    flight_sensor = FlightSensor("COM17")
    flight_sensor.connect()
    flight_sensor.imu_service.data_stream.subscribe(on_data)

    flight_sensor.stream(flight_sensor.Rate.RATE_5_HZ, flight_sensor.Flags.ALL)

    try:
        while True:
            pass
    except Exception:
        flight_sensor.disconnect()


if __name__ == '__main__':
    main()
