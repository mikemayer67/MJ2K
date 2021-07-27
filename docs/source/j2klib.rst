===============
J2KLib Overview
================

In simplest terms, the J2K Library (J2KLib) is a python wrapper around the
open source library OpenJPEG.  It provides a simplified Python interface to the
JPEG 2000 compression and decompression functions provided by OpenJPEG.

JPEG2000 is an image compression standard developed by the Joint Photographic 
Experts Group.  Unlike the original JPEG format, JPEG2000 uses a wavelet based
transform rather than a discrete cosine transform.  This has a number of desirable
properties including extraction of reduced sized images from a compressed image
through decompression of a subset of the wavelet coefficients and incremental 
refinement of image quality as additional bit layers are decoded.

OpenJPEG is an open source JPEG 2000 library written in C and compiled into a 
shared object library.  It is officially recognized by ISO/IEC as **the** reference
software for JPEG2000 codecs.

- The JPEG2000 standard is defined in the ISO/IEC 15444-1 specification.
- For a more thorough overview of JPEG 2000, see |WIKI_JPEG_2000|.
- For more information on OpenJPEG, see `OpenJPEG homepage <http://www.openjpeg.org>`_.

================
J2KLib Features
================

**Compression Types**

- Supports both numerically (NL) and visually (VL) lossless compression of images
- Supports decompression of both NL and VL compressed images

**Compression Parameters**

- Compression parameters may be specified as a predfined profile

  - NPJE (NSG Preferred JPEG Extension)
  - EPJE (Exploitation Preferred JPEG Extension) [*not yet implemented*]

- Compression parameters may be specified explicitly

  - Number of decomposition levels
  - Number of bit encoding layers
  - Strategy used to pack bits into layers

    - cumulative bits per pixel
    - cumulative PSNR (peak signal to noise ratio)

  - Bit packing order

- *Unspecified compression parameters default to their NPJE value*

**Image Data**

- Supports both single (e.g. monochromatic) and multi (e.g. RGB) component images
- Supports image data of arbitrary precesion (bits per pixel)
- Supports image data that is either signed or unsigned

**Tiled Images**

- Image data is provided as a list of tiles

- Pixel data for each tile is contained in a numpy array
- Pixel data for monochromatic images is specified in 2D arrays
- Pixel data for multi-component images is specified in 3D arrays

**J2K Codestreams**

- Compressed images are python bytes objects containing a J2K codestream

**Spark**

- J2KLib supports optional use of Spark to parallelize compression

  - Image tiles are input as an RDD rather than python list
  - Compressed imagery is returned as an RDD of J2K codestreams

- *Does not support use of Spark for decmpression*

===================
J2KLib Architecture
===================

=========
Interface
=========

=================
Technical Details
=================
