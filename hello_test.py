#!/usr/bin/env python

import ctypes
from ctypes import cdll
from ctypes import c_char_p, c_int, c_double
from ctypes import byref
from ctypes import create_string_buffer
from ctypes import util as cu

s = b'Hello, Mike\n'
cs = ctypes.create_string_buffer(s)

libc = cdll.LoadLibrary(cu.find_library('c'))

libc.printf(cs)

libc.printf.argtypes=[c_char_p,c_char_p,c_int,c_double]
libc.printf(b'%s %d %f\n',b'X',2,3.14)

hello = cdll.LoadLibrary('libhello.so')
hello.hello_world.restype = c_char_p
r = hello.hello_world()
print(r)

dest = create_string_buffer(32)
print(dest.raw)
hello.hello.restype = c_char_p
r = hello.hello(b"Nobody",byref(dest))
print(r)
print(dest.raw.decode('ascii'))

