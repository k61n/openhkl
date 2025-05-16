###### Qt
if(BUILD_WITH_QT6)
  set(QT_VER "Qt6")
else()
  set(QT_VER "Qt5")
endif()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(OpenGL_GL_PREFERENCE LEGACY)
set(QT_USE_QTOPENGL TRUE)
find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL:")
message(STATUS "  libraries: ${OPENGL_LIBRARIES}")
message(STATUS "  headers: ${OPENGL_INCLUDE_DIR}")

find_package(${QT_VER} COMPONENTS Core Gui Widgets OpenGL PrintSupport Concurrent REQUIRED)

set(QT_LIBS "${QT_VER}::Widgets")
set(QT_INCLUDES "${${QT_VER}Widgets_INCLUDE_DIRS}")

message(STATUS "Found ${QT_VER} version ${${QT_VER}_VERSION}:")
message(STATUS "  Qt_DIR: ${${QT_VER}_DIR}")
message(STATUS "  QT_INCLUDES: ${QT_INCLUDES}")

get_target_property(QtWidgets_location ${QT_VER}::Widgets LOCATION_Release)
message(STATUS "  ${${QT_VER}Widgets_LIBRARIES} ${QtWidgets_location}")
get_target_property(QtCore_location ${QT_VER}::Core LOCATION_Release)
message(STATUS "  ${${QT_VER}Core_LIBRARIES} ${QtCore_location}")
get_target_property(QtGui_location ${QT_VER}::Gui LOCATION_Release)
message(STATUS "  ${${QT_VER}Gui_LIBRARIES} ${QtGui_location}")
get_target_property(QtOpenGL_location ${QT_VER}::OpenGL LOCATION_Release)
message(STATUS "  ${${QT_VER}OpenGL_LIBRARIES} ${QtOpenGL_location}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

###### HDF5
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

###### Python
if(OHKL_PYTHON)
    set(pyver_min 3.9)
    find_package(Python3 ${pyver_min} QUIET
            COMPONENTS Interpreter Development NumPy)
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

###### C-Blosc
find_package(Blosc REQUIRED)
include_directories(SYSTEM ${Blosc_INCLUDE_DIRS})

###### Yaml-cpp
find_package(YAMLCPP REQUIRED)
include_directories(SYSTEM ${YAMLCPP_INCLUDES})

###### Tiff
find_package(TIFF REQUIRED)
include_directories(SYSTEM ${TIFF_INCLUDE_DIR})

##### FFTW
find_package(FFTW REQUIRED)
include_directories(SYSTEM ${FFTW_INCLUDE_DIR})

###### GSL
find_package(GSL REQUIRED)
include_directories(SYSTEM ${GSL_INCLUDE_DIR})

##### Eigen3
find_package(Eigen3 MODULE REQUIRED)
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

##### QHull
find_package(Qhull MODULE REQUIRED)
include_directories(SYSTEM ${QHULL_INCLUDE_DIR})

##### OpenCV
find_package(OpenCV REQUIRED)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
