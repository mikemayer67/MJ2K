#!/usr/bin/env python

import sys
import os
from PIL import Image
import numpy as np

def error(msg, usage=False):
    print()
    print(f'Sorry:: {msg}')
    print()
    if usage:
        print(f'Usage:: {sys.argv[0]} jpeg_filename [rd_filename]') 
        print()
    exit(1)

def main(args):
    narg = len(args)
    if narg == 0:
        error('Missing jpeg filename',usage=True)
    elif narg == 1:
        jpeg_file = args[0]
        rd_file   = None
    elif narg == 2:
        jpeg_file = args[0]
        rd_file   = args[1]
    else:
        error('Too many arguments',usage=True)

    if not os.path.exists(jpeg_file):
        error(f'Cannot find input file {jpeg_file}')

    if rd_file is None:
        rd_file = f'{jpeg_file}.rd'

    pil = Image.open(jpeg_file)
    (ncol,nrow) = pil.size
    bands = pil.getbands()
    nbands = len(bands)
    print(f'size: {nrow}x{ncol} x {bands}')

    rd = nrow.to_bytes(2,sys.byteorder)
    rd = rd + ncol.to_bytes(2,sys.byteorder)
    rd = rd + nbands.to_bytes(2,sys.byteorder)

    for (i,band) in enumerate(bands):
        pixels = np.array(pil.getdata(i),dtype=np.uint32)
        rd = rd + pixels.tobytes()

    with open(rd_file,"wb") as fd:
        fd.write(rd)

if __name__ == '__main__':
    main(sys.argv[1:])
