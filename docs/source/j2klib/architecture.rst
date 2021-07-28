J2KLib Architecture
===================

The J2K Library is a mixture of Python modules and compiled C libraries.
This leads to a notably different architecture for this package than for other 
CPA packages which are exclusivly Python.  This manifests itself most notably
in its internal interfaces and in the need to explicitly perform memory management
of any data allocated within the C code.

Primary Interfaces
------------------

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
library on whatever system J2KLib is to be used.  Like OpenJPEG, libj2k.so
is written in C and compiled as a dynamically linked library. It serves to
main functions: memory management and simplicification of the OpenJPEG API.

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

The OpenJPEG interface relies heavily on the passing of pointers to dynamically allocated
memory.  This includes both imagery and compression/decompression configuration parameters.
Some of this memory must be allocted and managed outside of OpenJPEG and passed to it. 
Some of this memory is allocted by OpenJPEG and must be subsequently freed by passing
it back to OpenJPEG for deallocation. Simply using `free` may lead to memory leaks as
the allocated block of memory may contain pointers to other blocks of memory allocated
by OpenJPEG.

As python's memory management model relies on garbage collection and not on direct
access to pointers to memory, the management of pointers to dynamically allocated 
memory is handled by libj2k.so.  To avoid inadvertent corruption of the memory these
pointers are never exposed through the public interface.  Instead, libj2k.so maps
each pointer to a data ID---an integer value that will be unique throughout the
entire lifespan of the process linked to libj2k.so.   When the memory associated
with a given data ID is no longer needed, the appropriate libj2k.so *free* 
function is called to either free the memory directly or invoke the necessary
OpenJPEG function to free it.  Attempting to free a given data ID more than once
is not an issue.  Only the first call will release memory.  All subsequent
calls to free the given data ID will simply do nothing.

The final layer of J2K memory management uses python's garbage collection.
Within :py:mod:`cpaj2k.j2klib.clibj2k`, any data ID returned from libj2k.so
is converted to an instance of :py:mod:`cpaj2k.j2klib.XXX?.DataId`.  This class
implements the :py:meth:`__del__` method, which invokes the appropriate libj2k.so *free*
function with the data ID associated with a *DataId* when it is no longer referenced 
within the j2klib python code.

.. figure:: /images/j2klib_memory.png
   :align: center
.. centered:: Figure 2. J2KLib DataId Lifecycle


Modules
-------

The figures and discussion above showed only the j2klib packages and modules
involved with the primary external interfaces and memory management.  Figure 3 below shows
all of the modules in cpaj2k.j2klib.  It also shows some additional modules in the
public API that were not discussed above:

- :py:mod:`cpaj2k.j2klib.spark.j2k_rdd_codestream` provides convenience function for converting 
  an RDD of J2K codestreams into a single python bytes object.

- :py:mod:`cpaj2k.j2klib.spark.s3` provides convenience functions for preparing or writing an 
  RDD of J2K codestreams to an S3 file.

- :py:mod:`cpaj2k.j2klib.error` defines the `J2KError` class used when a J2KLib function
  raises an exception.

- :py:mod:`cpaj2k.j2klib.j2k_version` provides a function for querying the version
  of OpenJPEG to which the J2KLib is currently linked.

.. figure:: /images/j2klib_modules.png
    :align: center
.. centered:: Figure 3. J2KLib Python Modules

Links to the documentation for these modules follow:

.. toctree::
   :maxdepth: 2

   /modules.rst


After figure

