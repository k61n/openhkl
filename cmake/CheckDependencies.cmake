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
if(OHKL_PYTHON)
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
        "from distutils import sysconfig as sc; print(sc.get_python_lib(prefix='', plat_specific=True))"
        RESULT_VARIABLE PYTHON_SITE_RESULT
        OUTPUT_VARIABLE PYTHON_SITE
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(PYTHON_SITE_RESULT)
        message(FATAL_ERROR "Failed running PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE} to determine"
            " Python package directory; output = '${PYTHON_SITE}'")
    endif()
    message(STATUS "Python package destination is ${PYTHON_SITE}")

    # swig
    find_package(SWIG REQUIRED)
    include(UseSWIG)
endif(OHKL_PYTHON)

###### Find C-Blosc
find_package(Blosc REQUIRED)
include_directories(SYSTEM ${Blosc_INCLUDE_DIRS})

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

##### Find Eigen3
find_package(Eigen3 MODULE REQUIRED)
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

##### Find QHull
find_package(Qhull MODULE REQUIRED)
include_directories(SYSTEM ${QHULL_INCLUDE_DIR})
