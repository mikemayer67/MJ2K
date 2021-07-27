J2KLib Architecture
===================

The J2K Library is a mixture of Python modules and compiled C libraries.
This leads to a notably different architecture for this package than for other 
CPA packages which are exclusivly Python.  This manifests itself most notably
in its internal interfaces and in the need to explicitly perform memory management
of any data allocated within the C code.

Interfaces
----------

In addition to the external interace exposed to clients of the library, there are a 
number of internal interfaces of interest as shown in Figure 1 below.

.. figure:: /images/j2klib_api.png
    :align: center
.. centered:: Figure 1. J2KLib Interfaces

The external interaces to the J2K Library are purely Python.  The three primary 
external interfaces are compression without Spark (:py:mod:`cpaj2k.j2klib.j2k_compress`),
compression with Spark (:py:mod:`cpaj2k.j2klib.spark.compress_j2k_rdd`), 
and decompression (:py:mod:`cpaj2k.j2klib.j2k_decompress`).
These are all invoked by an external app.

The first of the internal interfaces is a wrapper (*libj2k.so*) around 
the OpenJPEG library (*libopenjp2.so*).  OpenJPEG is, strictly speaking, 
not a part of J2KLib.  It must, however, be available as a dynamically linked
library on whatever system J2KLib is to be used.  There are two primary
functions of the libj2k.so library: memory management and simplicification
of the OpenJPEG API.

- Isolation of the J2KLib's python code from the issues of memory managment.

  - All allocation of dynamic memory passed to OpenJPEG is handled by libj2k.so
  - All freeing of dynamic memory returned by OpenJPEG is handled by libj2k.so
  - Pointers to dynamic memory are never exposed by the libj2k.so API.

- Simplification of the OpenJPEG API

  - OpenJPEG provides an extremely flexible interface for both compression and decompression.
  - This flexibility is made possible by complex data structures of configuration parameters.
  - J2KLib needs only a fraction of the flexibility OpenJPEG offers.
  - libj2k.so uses notably simpler data structures for setting configuration parameters.

The second of the internal interfaces is between the python code in cpaj2k.j2klib and the 
C code in the libj2k.so library.  The bridge between these two is handled by :py:mod:`cpaj2k.j2klib.clibj2k`, 
which in turn uses the :py:mod:`ctypes` module to handle the loading of libj2k.so and the invocation of 
functions within the libj2k.so library.  While use of :py:mod:`ctypes` make it possible to call C code, 
it does not necessary make the process simple.  A single invocation of a function in the libj2k.so 
library could take 20 or more lines of python. The role of :py:mod:`cpaj2k.j2klib.clibj2k` is to factor all of
the use of :py:mod:`ctypes` to a single module.  This 

- reduces redundant code scattered throughout the remainder of the j2klib code
- increases consistency in the use of :py:mod:`ctypes`
- improves maintainability of the remaining j2klib code as its logic is not intermixed with the overhead
  of using :py:mod:`ctypes`.

Memory Management
-----------------



Modules
-------

.. figure:: /images/j2klib_modules.png
    :align: center
.. centered:: Figure 2. J2KLib Python Modules


After figure

