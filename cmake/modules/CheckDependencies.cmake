###### Find boost libraries  ##############
find_package(Boost 1.54.0 COMPONENTS date_time system filesystem unit_test_framework REQUIRED)
if(Boost_FOUND)
    include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    message(STATUS "Found boost: " ${Boost_INCLUDE_DIRS})
    message(STATUS "Boost" ${Boost_LIBRARIES})
elseif(NOT Boost_FOUND)
    message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

###### Find OPENMP
if (BUILD_WITH_OPENMP)
  find_package(OpenMP REQUIRED)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

###### Find the Eigen3
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/externals/eigen3)

###### Search the HDF5 library
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

if ( HDF5_INCLUDE_DIRS AND HDF5_LIBRARIES )
   message("HDF5 found: include dir is ${HDF5_INCLUDE_DIRS} and library is ${HDF5_LIBRARIES} located in ${HDF5_LIBRARY_DIRS}")
else()
   messagE("ERROR: HDF5 not found")
endif()


###### C-BLOSC
add_subdirectory(externals/c-blosc)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/externals/c-blosc/blosc)

find_package(YAML REQUIRED)
include_directories(SYSTEM ${YAML_INCLUDES})

###### TIFF library
find_package(TIFF REQUIRED)
include_directories(SYSTEM ${TIFF_INCLUDE_DIR})

find_package(FFTW REQUIRED)
include_directories(SYSTEM ${FFTW_INCLUDE_DIR})

###### GSL library
if (BUILD_GSL)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/externals/gsl/cmake)
    add_subdirectory(externals/gsl)
    include_directories(SYSTEM ${CMAKE_BINARY_DIR}/externals/gsl/gsl)

    set (gsl_version_major 2)
    set (gsl_version_minor 3)

    set (GSL_LIBRARIES gsl gslcblas)
else()
    find_package(GSL 2.0 REQUIRED)
    include_directories(SYSTEM ${GSL_INCLUDE_DIR})

    set(temp_string ${GSL_VERSION})

    string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.([0-9]+))?" "\\1" gsl_version_major ${temp_string})
    string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.([0-9]+))?" "\\2" gsl_version_minor ${temp_string})
endif()

message("GSL_VERSION_MAJOR is ${gsl_version_major}")
message("GSL_VERSION_MINOR is ${gsl_version_minor}")

add_definitions(-DNSXTOOL_GSL_VERSION_MAJOR=${gsl_version_major})
add_definitions(-DNSXTOOL_GSL_VERSION_MINOR=${gsl_version_minor})
