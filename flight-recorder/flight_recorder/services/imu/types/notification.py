import logging
from enum import Enum

logger = logging.getLogger(__name__)


class Notification:
    class Type(Enum):
        STATE_UPDATE = 0
        SAMPLE = 1

    @classmethod
    def state_update(cls, previous, current):
        return cls(cls.Type.STATE_UPDATE, previous_state=previous, current_state=current)

    @classmethod
    def sample(cls, sample):
        return cls(cls.Type.SAMPLE, sample=sample)

    def __init__(self, notification_type, sample=None, previous_state=None, current_state=None):
        self.type = notification_type
        self.sample = sample
        self.previous_state = previous_state
        self.current_state = current_state
