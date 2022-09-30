import json


class CCppConfig:
    @classmethod
    def from_json(cls, path):
        with open(path) as json_file:
            info = json.load(json_file)
        return cls(path, info)

    def __init__(self, path, info):
        self._path = path
        self._info = info

    def insert_includes(self, includes):
        for c in self._info['configurations']:
            c['includePath'] = includes

    def insert_defines(self, defines):
        for c in self._info['configurations']:
            c['defines'] = defines

    def to_json(self):
        with open(self._path, "w") as json_file:
            json.dump(self._info, json_file, indent=4)