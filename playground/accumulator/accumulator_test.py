#!/usr/bin/env python

import ctypes
from ctypes import cdll
from ctypes import c_uint8, c_uint32
from ctypes import byref
from ctypes import create_string_buffer
from ctypes import util as cu
from ctypes import POINTER

import numpy as np

acl = cdll.LoadLibrary('libaccumulator.so')
acl.accumulate.restype = c_uint32
acl.accumulate.argtypes = [c_uint8, POINTER(c_uint32)];

n = np.random.randint(5,high=15, size=1,dtype=np.uint8)[0]
values = np.random.randint(0,high=1000, size=n,dtype=np.uint32)

ListType = c_uint32*20
c_values = ListType(*values)

rval = acl.accumulate(n, c_values)

lhs = ' + '.join((f'{i}' for i in values))
print(f'{lhs} = {rval}')

