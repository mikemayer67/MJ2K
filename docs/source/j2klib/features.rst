J2KLib Features
===============

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

