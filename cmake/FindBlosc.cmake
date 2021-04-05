
#[=======================================================================[.rst:

FindBlosc
---------

Finds the Blosc library.

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``Blosc_FOUND``
  True if the system has the Blosc library.
``Blosc_VERSION``
  The version of the Blosc library which was found.
``Blosc_INCLUDE_DIRS``
  Include directories needed to use Blosc.
``Blosc_LIBRARIES``
  Libraries needed to link to Blosc.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``Blosc_INCLUDE_DIR``
  The directory containing ``blosc.h``.
``Blosc_LIBRARY``
  The path to the Blosc library.

#]=======================================================================]


find_path(Blosc_INCLUDE_DIR blosc.h)

find_library(Blosc_LIBRARY NAMES blosc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Blosc DEFAULT_MSG Blosc_LIBRARY Blosc_INCLUDE_DIR)

# Create the variables required by CMake conventions
if(Blosc_FOUND)
  set(Blosc_LIBRARIES ${Blosc_LIBRARY})
  set(Blosc_INCLUDE_DIRS ${Blosc_INCLUDE_DIR})
endif()