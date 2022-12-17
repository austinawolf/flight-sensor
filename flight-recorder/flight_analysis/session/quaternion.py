import math
from flight_analysis.util.serializable import Serializable


class Quaternion(Serializable):
    q0: float
    q1: float
    q2: float
    q3: float

    @classmethod
    def from_euler(cls, roll, pitch, yaw):
        q0 = math.cos(roll / 2) * math.cos(pitch / 2) * math.cos(yaw / 2) + math.sin(roll / 2) * math.sin(
            pitch / 2) * math.sin(yaw / 2)
        q1 = math.sin(roll / 2) * math.cos(pitch / 2) * math.cos(yaw / 2) - math.cos(roll / 2) * math.sin(
            pitch / 2) * math.sin(yaw / 2)
        q2 = math.cos(roll / 2) * math.sin(pitch / 2) * math.cos(yaw / 2) + math.sin(roll / 2) * math.cos(
            pitch / 2) * math.sin(yaw / 2)
        q3 = math.cos(roll / 2) * math.cos(pitch / 2) * math.sin(yaw / 2) - math.sin(roll / 2) * math.sin(
            pitch / 2) * math.cos(yaw / 2)
        return cls(q0, q1, q2, q3)

    @classmethod
    def from_tuple(cls, q):
        return cls(q[0], q[1], q[2], q[3])

    def __init__(self, q0, q1, q2, q3):
        self.q0 = q0
        self.q1 = q1
        self.q2 = q2
        self.q3 = q3
        if None not in self.tuple:
            self.normalize()

    def normalize(self):
        magnitude = self.magnitude()
        try:
            self.q0 /= magnitude
            self.q1 /= magnitude
            self.q2 /= magnitude
            self.q3 /= magnitude
        except ZeroDivisionError:
            pass

    def magnitude(self):
        return math.sqrt(self.q0 ** 2 + self.q1 ** 2 + self.q2 ** 2 + self.q3 ** 2)

    @property
    def tuple(self):
        return self.q0, self.q1, self.q2, self.q3

    @property
    def roll(self):
        return math.atan2(2 * (self.q0 * self.q1 + self.q2 * self.q3),
                          1 - 2 * (self.q1 * self.q1 + self.q2 * self.q2)) * 57.29

    @property
    def pitch(self):
        return math.asin(2 * (self.q0 * self.q2 - self.q3 * self.q1)) * 57.29

    @property
    def yaw(self):
        return math.atan2(2 * (self.q0 * self.q3 + self.q1 * self.q2),
                          1 - 2 * (self.q2 * self.q2 + self.q3 * self.q3)) * 57.29
