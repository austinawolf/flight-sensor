import json
from datetime import datetime
from typing import List, Tuple, get_type_hints, get_args

import typing_utils


class Serializable:
    @classmethod
    def _deserialize(cls, attr_type, serialized):
        if typing_utils.issubtype(attr_type, List):
            inner_type = get_args(attr_type)[0]
            return [Serializable._deserialize(inner_type, s) for s in serialized]
        elif typing_utils.issubtype(attr_type, Tuple):
            inner_type = get_args(attr_type)[0]
            return tuple([Serializable._deserialize(inner_type, s) for s in serialized])
        elif typing_utils.issubtype(attr_type, Serializable):
            return attr_type.from_dict(serialized)
        elif typing_utils.issubtype(attr_type, datetime):
            return datetime.strptime(serialized, '%m/%d/%Y %H:%M:%S')
        elif attr_type in [float, int, str, bool]:
            return attr_type(serialized)
        else:
            raise TypeError(f"{serialized} of type {attr_type} is not serializable")

    @classmethod
    def from_dict(cls, attributes: dict):
        if hasattr(cls.__init__, "__code__"):
            args = tuple((cls.__init__.__code__.co_argcount - 1) * [None])
            obj = cls(*args)
        else:
            obj = cls()

        for name, attr_type in cls.__annotations__.items():
            if name in attributes:
                value = cls._deserialize(attr_type, attributes[name])
                setattr(obj, name, value)
        return obj

    @classmethod
    def from_json(cls, json_string):
        return json.loads(json_string)

    @classmethod
    def load(cls, path):
        with open(path, "r") as f:
            json_string = f.read()
            json_dict = cls.from_json(json_string)
            return cls.from_dict(json_dict)

    @staticmethod
    def _serialize(attr_type, value):
        if typing_utils.issubtype(attr_type, List):
            inner_type = get_args(attr_type)[0]
            return [Serializable._serialize(inner_type, v) for v in value]
        if typing_utils.issubtype(attr_type, Tuple):
            inner_type = get_args(attr_type)[0]
            return tuple([Serializable._serialize(inner_type, v) for v in value])
        elif typing_utils.issubtype(attr_type, Serializable):
            return value.to_dict()
        elif typing_utils.issubtype(attr_type, datetime):
            return value.strftime("%m/%d/%Y %H:%M:%S")
        elif attr_type in [float, int, str, bool]:
            return value
        else:
            raise TypeError(f"{value} of type {type(value)} is not serializable")

    def to_dict(self):
        items = {}
        for name, attr_type in self.__annotations__.items():
            items[name] = self._serialize(attr_type, getattr(self, name))
        return items

    def to_json(self):
        return json.dumps(self.to_dict(), indent=4)

    def save(self, path):
        with open(path, "w") as f:
            f.write(self.to_json())
