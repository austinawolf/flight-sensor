import os


class Defines:
    @classmethod
    def from_args(cls, text, prefix):
        defines_raw_list = text.split(" ")
        defines = []
        for item in defines_raw_list:
            if item.startswith(prefix):
                defines.append(item[len(prefix):])
        return cls(defines)

    def __init__(self, items):
        self.items = items


class Includes:
    @classmethod
    def from_args(cls, text):
        include_raw_list = text.split(" ")
        includes = []
        for item in include_raw_list:
            absolute_path = os.path.abspath(item)
            includes.append(absolute_path)
        return cls(includes)

    def __init__(self, items):
        self.items = items
