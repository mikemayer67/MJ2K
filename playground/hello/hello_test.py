#!/usr/bin/env python

import ctypes
from ctypes import cdll
from ctypes import c_char_p, c_int, c_double
from ctypes import c_uint8, c_uint16, c_uint32
from ctypes import byref
from ctypes import create_string_buffer
from ctypes import util as cu
from ctypes import POINTER

import numpy as np

s = b'Hello, Mike\n'
cs = ctypes.create_string_buffer(s)

libc = cdll.LoadLibrary(cu.find_library('c'))

libc.printf(cs)

libc.printf.argtypes=[c_char_p,c_char_p,c_int,c_double]
libc.printf(b'%s %d %f\n',b'X',2,3.14)

hello = cdll.LoadLibrary('libhello.dylib')
hello.hello_world.restype = c_char_p
r = hello.hello_world()
print(r)

dest = create_string_buffer(32)
print(dest.raw)
hello.hello.restype = c_char_p
r = hello.hello(b"Nobody",byref(dest))
print(r)
print(dest.raw.decode('ascii'))

print()
print('------------------------------------------')
print('call C-library function that returns pointer to uint16_t array')
print('  uint32_t *gen_data(uint16_t)')
print('------------------------------------------')
print()

hello.gen_data.argtypes=[c_uint16];
hello.gen_data.restype = POINTER(c_uint32)
data = hello.gen_data(24);
print(data)
print(type(data))

q = np.ctypeslib.as_array(data,shape=(24,))
print(type(q))
print(q.shape)
print(q.dtype)
print(q)

