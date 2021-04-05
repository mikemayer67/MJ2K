#!/usr/bin/env python3

import os,glob
import numpy as np
from PIL import Image

import ctypes.util
from ctypes import pointer,byref
from ctypes.util import find_library
from mamj2k import *

import time

def ellapsed(last_time):
    cur_time = time.perf_counter()
    print(f'Ellapsed: {cur_time-last_time}')
    return cur_time

last_time = time.perf_counter()

mamj2k_path = find_library('mamj2k')
print(f'Loading MAMJ2K library from {mamj2k_path}')
mj2k = cdll.LoadLibrary(mamj2k_path)

last_time = ellapsed(last_time)

mj2k.mj2k_opj_version.restype = c_char_p
ver = mj2k.mj2k_opj_version().decode('utf-8')
print(ver)

last_time = ellapsed(last_time)

Image.MAX_IMAGE_PIXELS = int(256512000/2)

jpg_file = 'StockPhotos/frog_dog2x2.jpg'
pil = Image.open(jpg_file)
(ncol,nrow) = pil.size
print(f'size: {nrow}x{ncol}')

last_time = ellapsed(last_time)

r = np.array(pil.getdata(0),dtype=np.uint8).reshape((nrow,ncol))
print(f'extracted red')

last_time = ellapsed(last_time)

mj2k.mj2k_write_j2k.argtypes = [POINTER(MJ2K_IMAGE_T), POINTER(MJ2K_CPARAM_T), c_char_p]
mj2k.mj2k_read_j2k.restype = ctypes.c_int

image = MJ2K_IMAGE_T()
image.bw_from_array(r)

cparam = MJ2K_CPARAM_T()

j2k_root = os.path.splitext(jpg_file)[0]

print(f'created MJ2K image')
last_time = ellapsed(last_time)

cparam.irreversible = 1
cparam.tcp_numlayers = 2
cparam.numresolution = 6
cparam.qual_type = 0
cparam.layer_qual_value[0] = 20
cparam.layer_qual_value[1] = 0

j2k_filename = f'{j2k_root}_vl_cp2_psnr20.j2k'
j2k_file = c_char_p(j2k_filename.encode('utf-8'))

mj2k.mj2k_write_j2k(image,cparam,j2k_file)

print(f'created {j2k_file}')
last_time = ellapsed(last_time)

mj2k.mj2k_read_j2k.argtypes = [c_char_p]
mj2k.mj2k_read_j2k.restype = POINTER(MJ2K_IMAGE_T)

image_p = mj2k.mj2k_read_j2k(j2k_file)
print(f'read {j2k_file}')
last_time = ellapsed(last_time)

image = image_p.contents
print(f'extracted image contents')
last_time = ellapsed(last_time)


