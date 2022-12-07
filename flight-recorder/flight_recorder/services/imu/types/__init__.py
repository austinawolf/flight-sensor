from enum import Enum


class SessionState(Enum):
    IDLE = 0
    STREAMING = 1
    RECORDING = 2
    PLAYBACK = 3
    CALIBRATING = 4
    ERROR = 5
