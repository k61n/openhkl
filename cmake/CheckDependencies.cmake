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
find_library(HDF5_LIB_PATH libhdf5_cpp)

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
    message("  CXX library: ${HDF5_CXX_LIBRARY_hdf5_cpp}")
else()
    message(FATAL_ERROR "HDF5 not found")
endif()

###### Find Python
if(OHKL_PYTHON)
    set(pyver_min 3.9)
    find_package(Python3 ${pyver_min} QUIET
            COMPONENTS Interpreter Development  )
    if(NOT Python3_FOUND)
        message(FATAL_ERROR
            "Python 3 not found (minimum version ${pyver_min}).")
    endif()

    message(STATUS "   Python3_VERSION: ${Python3_VERSION}")
    message(STATUS "   Python3_VERSION_MINOR: ${Python3_VERSION_MINOR}")
    message(STATUS "   Python3_VERSION_PATCH: ${Python3_VERSION_PATCH}")
    message(STATUS "   Python3_INTERPRETER_ID: ${Python3_INTERPRETER_ID}")
    message(STATUS "   Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
    message(STATUS "   Python3_STDLIB: ${Python3_STDLIB}")
    message(STATUS "   Python3_SITELIB: ${Python3_SITELIB}")
    message(STATUS "   Python3_INCLUDE_DIRS: ${Python3_INCLUDE_DIRS}")
    message(STATUS "   Python3_LIBRARIES: ${Python3_LIBRARIES}")
    message(STATUS "   Python3_LIBRARY_RELEASE: ${Python3_LIBRARY_RELEASE}")
    if(WIN32)
        message(STATUS "   Python3_LIBRARY_DLL: ${Python3_LIBRARY_DLL}")
    endif(WIN32)
    message(STATUS "   Python3_LIBRARY_DIRS: ${Python3_LIBRARY_DIRS}")
    message(STATUS "   Python3_NumPy_VERSION: ${Python3_NumPy_VERSION}")
    message(STATUS "   Python3_NumPy_INCLUDE_DIRS: ${Python3_NumPy_INCLUDE_DIRS}")

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

##### Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
