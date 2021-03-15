import ctypes
from ctypes import cdll
from ctypes import Structure, POINTER, pointer
from ctypes import c_char_p, c_uint8, c_uint32, c_int64, c_float

import numpy as np
from PIL import Image

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

    def from_array(self,pixels):
        # pixels is a numpy array
        self.x0 = 0
        self.y0 = 0
        (self.h,self.w) = pixels.shape
        self.dx = 1
        self.dy = 1
        self.pixels = pixels.ctypes.data_as(POINTER(c_uint8))

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

    def rgb_from_array(self,r,g,b):
        self.x0 = 0
        self.y0 = 0
        (self.y1,self.x1) = r.shape
        self.ncomp = 3
        c = (MJ2K_COMP_T * 3)()
        c[0].from_array(r)
        c[1].from_array(g)
        c[2].from_array(b)
        self.comp = ctypes.cast(c, POINTER(MJ2K_COMP_T))

    def bw_from_array(self,pixels):
        self.x0 = 0
        self.y0 = 0
        (self.y1,self.x1) = pixels.shape
        self.ncomp = 1
        
        c = MJ2K_COMP_T()
        c.from_array(pixels)
        self.comp = pointer(c)

    def grayscale(self):
        return self.ncomp == 1

    def rgb(self):
        return self.ncomp ==3 and \
                self.comp[0].rgb_compatible(self.comp[1]) and \
                self.comp[0].rgb_compatible(self.comp[2])

    def display_rgb(self):
        c = self.comp[0]
        w = c.w
        h = c.h
        n = w * h
        rgb = ( self.comp[i] for i in range(3) )
        rgb = ( ctypes.cast(c.pixels, POINTER(c_uint8 * n)).contents for c in rgb )
        rgb = [ np.array(p).reshape((h,w)) for p in rgb ]
        rgb = np.dstack(rgb)
        display(Image.fromarray(rgb))

    def display_bw(self,comp=0):
        comp = self.comp[comp]
        w = comp.w
        h = comp.h
        n = w * h
        pixels = ctypes.cast(comp.pixels, POINTER(c_uint8 * n)).contents
        pixels = np.array(pixels).reshape((h,w))
        display(Image.fromarray(pixels))

    def display(self):
        if self.rgb():
            print("DISPLAY RGB")
            self.display_rgb()
        elif self.grayscale():
            print("DISPLAY GRAYSCALE")
            self.display_bw(0)
        else:
            print("DISPLAY COMPONENTS")
            for i in range(self.ncomp):
                print(f'COMP {i+1}')
                self.display(i)

class MJ2K_CPARAM_T (Structure):
    _fields_ = [
        ('irreversible',    c_uint8),
        ('tcp_numlayers',   c_uint8),
        ('numresolution',   c_uint8),
        ('layer_qual_type', c_uint8),
        ('layer_qual_value', c_float * 100),
    ]
