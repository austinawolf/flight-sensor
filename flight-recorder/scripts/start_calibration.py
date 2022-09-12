import time
from flight_recorder.flight_sensor import FlightSensor


def main():
    flight_sensor = FlightSensor("COM17")
    flight_sensor.initialize()
    time.sleep(5)
    flight_sensor.deintialize()


if __name__ == '__main__':
    main()
