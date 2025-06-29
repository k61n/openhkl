.. _install:

Installation guide
==================

.. _compile_instructions:

Compile from source
-------------------

1. Install dependencies.

   * C++ compiler (e.g. g++, clang)
   * Python3
   * Cmake
   * QT5/6
   * Eigen3
   * GSL
   * HDF5
   * OpenGL
   * vtk
   * Tiff
   * QHull
   * YAMLCpp
   * FFTW3
   * Blosc
   * OpenCV
   * Swig
   * numpy
   * nlopt

2. Clone the git repository

::

   git clone https://iffgit.fz-juelich.de/neutron-simlab/openhkl.git

3. Create a build directory, run ``cmake`` and compile the source:

::

   cd openhkl
   cmake -B build .
   cd build
   make -j8
   make install <optional>

4. A SWIG-generated python module is installed to `build/swig/pyohkl.py`;
   in order to access OpenHKL libraries from a python script, it is necessary to
   either add the `build/swig` directory to your `$PYTHONPATH` variable, or add
   the following lines to your script.

::

   import sys
   sys.path.append("/path/to/build/swig")
   import pyohkl as ohkl
