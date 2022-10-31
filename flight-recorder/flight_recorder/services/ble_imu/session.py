import logging
import math
from datetime import datetime
from typing import List
import xlsxwriter
from xlsxwriter.worksheet import Worksheet

from flight_recorder.packets.data import Data

logger = logging.getLogger(__name__)


class Quaternion:
    @classmethod
    def from_euler(cls, roll, pitch, yaw):
        q0 = math.cos(roll / 2) * math.cos(pitch / 2) * math.cos(yaw / 2) + math.sin(roll / 2) * math.sin(pitch / 2) * math.sin(yaw / 2)
        q1 = math.sin(roll / 2) * math.cos(pitch / 2) * math.cos(yaw / 2) - math.cos(roll / 2) * math.sin(pitch / 2) * math.sin(yaw / 2)
        q2 = math.cos(roll / 2) * math.sin(pitch / 2) * math.cos(yaw / 2) + math.sin(roll / 2) * math.cos(pitch / 2) * math.sin(yaw / 2)
        q3 = math.cos(roll / 2) * math.cos(pitch / 2) * math.sin(yaw / 2) - math.sin(roll / 2) * math.sin(pitch / 2) * math.cos(yaw / 2)
        return cls((q0, q1, q2, q3))

    def __init__(self, quat):
        self.q0 = quat[0]
        self.q1 = quat[1]
        self.q2 = quat[2]
        self.q3 = quat[3]

    def normalize(self):
        magnitude = self.magnitude()
        self.q0 /= magnitude
        self.q1 /= magnitude
        self.q2 /= magnitude
        self.q3 /= magnitude

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


class BleImuSession:
    def __init__(self, timestamp: datetime, session_time: int, sampling_rate, samples: List[Data]):
        self.timestamp = timestamp
        self.session_time = session_time
        self.sampling_rate = sampling_rate
        self.samples = samples

    def save_report(self, filename):
        workbook = xlsxwriter.Workbook(filename)
        data_sheet = workbook.add_worksheet()
        self._write_data(data_sheet, self.samples[:-2])

        info_sheet = workbook.add_worksheet()
        self._write_info(info_sheet, self.timestamp, self.session_time, self.sampling_rate)

        workbook.close()

        logger.info(f"Wrote {len(self.samples)} to report: {filename}")

    @staticmethod
    def _write_info(sheet: Worksheet, timestamp, session_time, sampling_rate):
        sheet.write_row(0, 0, ["Playback Timestamp", timestamp])
        sheet.write_row(1, 0, ["Session Time", session_time])
        sheet.write_row(2, 0, ["Sampling Rate", sampling_rate])

    @staticmethod
    def _write_data(sheet: Worksheet, samples: List[Data]):
        headers = ["timestamp", "flags", "index",
                   "gyro_x", "gyro_y", "gyro_z",
                   "accel_x", "accel_y", "accel_z",
                   "compass_x", "compass_y", "compass_z",
                   "q0", "q1", "q2", "q3", "Roll", "Pitch", "Yaw"]

        sheet.write_row(0, 0, headers)

        for i, sample in enumerate(samples):
            quat = Quaternion(sample.quat)
            quat.normalize()

            items = [
                sample.timestamp,
                sample.flags,
                sample.index,
                sample.gyro[0],
                sample.gyro[1],
                sample.gyro[2],
                sample.accel[0],
                sample.accel[1],
                sample.accel[2],
                sample.compass[0],
                sample.compass[1],
                sample.compass[2],
                sample.quat[0],
                sample.quat[1],
                sample.quat[2],
                sample.quat[3],
                quat.roll,
                quat.pitch,
                quat.yaw,
            ]

            sheet.write_row(i + 1, 0, items)
