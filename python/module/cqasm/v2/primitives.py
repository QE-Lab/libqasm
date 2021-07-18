from enum import Enum
import numpy as np

Str = str
Bool = bool
Int = int
Real = float
Complex = complex

class Version(tuple):
    """cQASM file version number."""

    def __new__(cls, elements):
        if isinstance(elements, str):
            elements = elements.split('.')
        return super(Version, cls).__new__(cls, tuple(map(int, elements)))

def serialize(typ, val):
    if isinstance(typ, str):
        return None
    elif typ is Str:
        return {'x': val}
    elif typ is Bool:
        return {'x': val}
    elif typ is Int:
        return {'x': val}
    elif typ is Real:
        return {'x': val}
    elif typ is Complex:
        return {'r': val.real, 'i': val.imag}
    elif typ is Version:
        return {'x': list(val)}
    else:
        assert False

def deserialize(typ, val):
    if isinstance(typ, str):
        return None
    elif typ is Str:
        return Str(val['x'])
    elif typ is Bool:
        return Bool(val['x'])
    elif typ is Int:
        return Int(val['x'])
    elif typ is Real:
        return Real(val['x'])
    elif typ is Complex:
        return Complex(val['r'], val['i'])
    elif typ is Version:
        return Version(val['x'])
    else:
        assert False
