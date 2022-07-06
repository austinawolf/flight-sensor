import struct


class MotionData:
    MINIMUM_LEN = 3
    PREAMBLE = 0xAA

    class Format:
        HEADER = "<BBB"
        QUAT = HEADER + "iiii"
        IMU = HEADER + "hhhhhh"
        COMPASS = HEADER + "hhh"

    class Flags:
        QUAT = 0b001
        IMU = 0b010
        COMPASS = 0b100

    @classmethod
    def from_bytes(cls, _bytes):
        if len(_bytes) < cls.MINIMUM_LEN:
            raise Exception

        header = _bytes[:cls.MINIMUM_LEN]

        preamble, flags, sample_number = struct.unpack(cls.Format.HEADER, header)
        if preamble != cls.PREAMBLE:
            raise Exception

        if flags == cls.Flags.QUAT:
            return cls.parse_quat(_bytes)
        elif flags == cls.Flags.IMU:
            return cls.parse_imu(_bytes)
        elif flags == cls.Flags.COMPASS:
            return cls.parse_compass(_bytes)
        else:
            print(f"Unexpected value: {flags}")
        return None

    @classmethod
    def parse_imu(cls, _bytes):
        preamble, flags, sample_number, \
            accel_x, accel_y, accel_z, \
            gyro_x, gyro_y, gyro_z = struct.unpack(cls.Format.IMU, _bytes)
        return cls(sample_number, gyro=(gyro_x, gyro_y, gyro_z), accel=(accel_x, accel_y, accel_z))

    @classmethod
    def parse_compass(cls, _bytes):
        preamble, flags, sample_number, \
            compass_x, compass_y, compass_z = struct.unpack(cls.Format.COMPASS, _bytes)
        return cls(sample_number, compass=(compass_x, compass_y, compass_z))

    @classmethod
    def parse_quat(cls, _bytes):
        preamble, flags, sample_number, \
            quat_w, quat_x, quat_y, quat_z = struct.unpack(cls.Format.QUAT, _bytes)
        return cls(sample_number, quat=(quat_w, quat_x, quat_y, quat_z))

    def __init__(self, sample_number,
                 quat=None,
                 gyro=None,
                 accel=None,
                 compass=None):
        self.sample_number = sample_number
        self.quat = quat
        self.gyro = gyro
        self.accel = accel
        self.compass = compass

    @property
    def has_quat(self):
        return self.quat is not None

    @property
    def has_gyro(self):
        return self.gyro is not None

    @property
    def has_accel(self):
        return self.accel is not None

    @property
    def has_compass(self):
        return self.compass is not None
