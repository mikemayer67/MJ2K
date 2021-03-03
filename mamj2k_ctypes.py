import ctypes
from ctypes import cdll
from ctypes import Structure, POINTER
from ctypes import c_char_p, c_uint8, c_uint32, c_int64

class MJ2K_COMP_T (Structure):
    _fields_ = [
        ('x0', c_uint32),
        ('y0', c_uint32),
        ('w',  c_uint32),
        ('h',  c_uint32),
        ('dx', c_uint32),
        ('dy', c_uint32),
        ('pixels', POINTER(c_uint8)),
    ]

    def rgb_compatible(self,other):
        if self.w != other.w:
            return False
        if self.h != other.h:
            return False
        if self.x0 != other.x0:
            return False
        if self.y0 != other.y0:
            return False
        if self.dx != other.dx:
            return False
        if self.dy != other.dy:
            return False
        return True


class MJ2K_IMAGE_T (Structure):
    _fields_ = [
        ('x0', c_uint32),
        ('y0', c_uint32),
        ('x1', c_uint32),
        ('y1', c_uint32),
        ('ncomp', c_uint32),
        ('comp', POINTER(MJ2K_COMP_T)),
    ]



