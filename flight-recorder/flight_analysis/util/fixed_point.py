
def fixed_to_float(value, bits, fsr, signed=True) -> float:
    fixed_range = pow(2, bits) / 2 - 1 if signed else pow(2, bits) - 1
    return value * (fsr / fixed_range)


def float_to_fixed(value, fsr, fixed_range) -> int:
    raise NotImplementedError