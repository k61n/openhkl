###### Find boost
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.53.0 REQUIRED)
if(Boost_FOUND)
    include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
    message(STATUS "Found boost:")
    message(STATUS "  version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}")
    message(STATUS "  libraries: ${Boost_LIBRARIES}")
    message(STATUS "  headers: ${Boost_INCLUDE_DIRS}")
elseif(NOT Boost_FOUND)
    message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

###### Find OPENMP
if(BUILD_WITH_OPENMP)
    find_package(OpenMP REQUIRED)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

###### Find Eigen3
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

###### Find HDF5
find_package(HDF5 COMPONENTS CXX REQUIRED)

# manually fix problem with windows build
if(NOT HDF5_LIBRARIES)
    set(HDF5_LIBRARIES "")
    find_library(HDF5_LIBRARY_TEMP libszip)
    set(HDF5_LIBRARIES "${HDF5_LIBRARIES};${HDF5_LIBRARY_TEMP}")

    find_library(HDF5_LIBRARY_TEMP libzlib)
    set(HDF5_LIBRARIES "${HDF5_LIBRARIES};${HDF5_LIBRARY_TEMP}")

    find_library(HDF5_LIBRARY_TEMP libhdf5)
    set(HDF5_LIBRARIES "${HDF5_LIBRARIES};${HDF5_LIBRARY_TEMP}")

    find_library(HDF5_LIBRARY_TEMP libhdf5_cpp)
    set(HDF5_LIBRARIES "${HDF5_LIBRARIES};${HDF5_LIBRARY_TEMP}")
endif()

include_directories(SYSTEM "${HDF5_INCLUDE_DIRS}")

if(HDF5_INCLUDE_DIRS AND HDF5_LIBRARIES)
    message("HDF5 found:")
    message("  versions: ${HDF5_VERSION}")
    message("  libraries: ${HDF5_LIBRARIES}")
    message("  headers: ${HDF5_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "HDF5 not found")
endif()

###### Find Python
if(NSX_PYTHON)
    # python-dev and interpreter
    set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4)
    find_package(PythonInterp 3)
    find_package(PythonLibs 3)

    execute_process (
        COMMAND ${PYTHON_EXECUTABLE} -c "from __future__ import print_function; import numpy; print(numpy.get_include())"
        ERROR_VARIABLE NUMPY_FIND_ERROR
        RESULT_VARIABLE NUMPY_FIND_RESULT
        OUTPUT_VARIABLE NUMPY_FIND_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    ## process the output from the execution of the command
    if(NOT NUMPY_FIND_RESULT)
        set (NUMPY_INCLUDES ${NUMPY_FIND_OUTPUT})
        message(STATUS "numpy includes ${NUMPY_INCLUDES}")
        include_directories(SYSTEM ${NUMPY_INCLUDES})
    else()
        message(FATAL_ERROR "Could NOT find numpy headers")
    endif()

    # Python packages dir
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
        "from __future__ import print_function; from distutils import sysconfig as sc; print(sc.get_python_lib(prefix='', plat_specific=True))"
        RESULT_VARIABLE PYTHON_SITE_RESULT
        OUTPUT_VARIABLE PYTHON_SITE
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT PYTHON_SITE_RESULT)
        message(STATUS "python package destination is ${PYTHON_SITE}")
    else()
        message(FATAL_ERROR "could NOT determine python package directory")
    endif()

    # swig
    find_package(SWIG REQUIRED)
    include(UseSWIG)
endif(NSX_PYTHON)

###### Find C-Blosc
find_package(Blosc REQUIRED)
include_directories(SYSTEM ${Blosc_INCLUDES})

message("Blosc_INCLUDE_DIR: ${Blosc_INCLUDE_DIR}")
message("Blosc_INCLUDE_DIRS: ${Blosc_INCLUDE_DIRS}")
message("Blosc_INCLUDES: ${Blosc_INCLUDES}")

find_package(YAMLCPP REQUIRED)
include_directories(SYSTEM ${YAMLCPP_INCLUDES})

###### Find TIFF
find_package(TIFF REQUIRED)
include_directories(SYSTEM ${TIFF_INCLUDE_DIR})

find_package(FFTW REQUIRED)
include_directories(SYSTEM ${FFTW_INCLUDE_DIR})

###### Find GSL
find_package(GSL REQUIRED)
include_directories(SYSTEM ${GSL_INCLUDE_DIR})

#find_package(XSection REQUIRED)
#include_directories(SYSTEM ${XSection_INCLUDE_DIR})
