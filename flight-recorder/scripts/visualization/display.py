import time
from flight_analysis.factory.animator_factory import AnimatorConfig, AnimatorFactory
from flight_analysis.fusion.madgwick.madgwick import MadgwickAHRS
from flight_analysis.session.quaternion import Quaternion
from flight_analysis.session.sample import Sample
from flight_recorder.flight_sensor import FlightSensor


SESSION_TIME = 10
OUTPUT_DIR = "recordings"


def main():
    config = AnimatorConfig(lib="pyvista", enable_sliders=False)
    factory = AnimatorFactory(config)
    fusion = MadgwickAHRS(0.01, 0.2, 0.0)

    animator = factory.create_animator()
    animator.start()

    flight_sensor = FlightSensor("COM17")

    def on_sample(new_sample):
        if not new_sample:
            return

        quaternion = Quaternion(new_sample.quat[0], new_sample.quat[1], new_sample.quat[2], new_sample.quat[3])
        sample = Sample.from_icm20948(new_sample.timestamp, new_sample.flags,
                                      new_sample.accel, new_sample.gyro,
                                      new_sample.compass, quaternion)
        fusion.update(sample.gyro, sample.accel, sample.compass)
        output = fusion.quaternion
        print(output.q)
        animator.set_orientation(output.q)

    flight_sensor.connect()
    imu_service = flight_sensor.imu_service
    imu_service.on_sample.subscribe(on_sample)
    imu_service.stream(flight_sensor.Rate.RATE_10_HZ, flight_sensor.Flags.ALL, SESSION_TIME)

    try:
        if SESSION_TIME:
            time.sleep(SESSION_TIME)
        else:
            while True:
                pass
    except KeyboardInterrupt:
        imu_service.stop()

    imu_service.wait_for_idle()

    flight_sensor.disconnect()


if __name__ == '__main__':
    main()
