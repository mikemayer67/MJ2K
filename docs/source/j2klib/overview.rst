J2KLib Overview
===============

In simplest terms, the J2K Library (J2KLib) is a python wrapper around the 
open source library OpenJPEG. It provides a simplified Python interface to 
the JPEG 2000 compression and decompression functions provided by OpenJPEG.

JPEG2000 is an image compression standard developed by the Joint Photographic 
Experts Group. Unlike the original JPEG format, JPEG2000 uses a wavelet based 
transform rather than a discrete cosine transform. This has a number of desirable 
properties including extraction of reduced sized images from a compressed image 
through decompression of a subset of the wavelet coefficients and incremental 
refinement of image quality as additional bit layers are decoded.

OpenJPEG is an open source JPEG 2000 library written in C and compiled 
into a shared object library. It is officially recognized by ISO/IEC 
as **the** reference software for JPEG2000 codecs.

- The JPEG2000 standard is defined in the ISO/IEC 15444-1 specification.
- For a more thorough overview of JPEG 2000, see |WIKI_JPEG_2000|
- For more information on OpenJPEG, see the `OpenJPEG homepage <http://www.openjpeg.org>`_.

