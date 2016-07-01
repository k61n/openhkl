cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/externals/c-blosc/cmake)

project(NSXTOOL_PROJECT)

include(CMakeFindBinUtils)

set(NSXTOOL_VERSION_MAJOR 1)
set(NSXTOOL_VERSION_MINOR 0)

# Propose two build configurations
#set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Compilation configuration" FORCE)
if(DEFINED CMAKE_BUILD_TYPE)
    set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release" )
endif()

###########################################################################
# Options
###########################################################################

option(BUILD_QTAPPS "Build Qt based application." ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# additional macros
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
include(CheckCompiler)

#Set package name here
set(CPACK_PACKAGE_NAME "nsxtool")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neutron Single Crystal Diffraction Data Reduction")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "${NSXTOOL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${NSXTOOL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;" )

###########################################################################
# Check dependencies
###########################################################################

###### Find boost libraries  ##############
find_package(Boost 1.54.0 COMPONENTS date_time system filesystem unit_test_framework REQUIRED)
if(Boost_FOUND)
    include_directories("${Boost_INCLUDE_DIRS}")
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    message(STATUS "Found boost: " ${Boost_INCLUDE_DIRS})
    message(STATUS "Boost" ${Boost_LIBRARIES})
elseif(NOT Boost_FOUND)
    message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

###### Find OPENMP 
find_package(OpenMP)

###### Find the Eigen3 
find_package(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

###### Search the HDF5 library
find_package(HDF5 COMPONENTS CXX)

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

include_directories("${HDF5_INCLUDE_DIR}")

if ( HDF5_INCLUDE_DIR AND HDF5_LIBRARIES )
   message("HDF5 found: include dir is ${HDF5_INCLUDE_DIR} and library is ${HDF5_LIBRARIES} located in ${HDF5_LIBRARY_DIR}")
else()
   messagE("ERROR: HDF5 not found")   
endif()


find_package(Blosc REQUIRED)
if (BLOSC_FOUND==True)
        message("Blosc library found, already installed on the system")
else()
        #### Internal dependencies
        add_subdirectory(externals/c-blosc)
        message("Blosc library not found, will build from internal resources")
endif()

###### TIFF library
find_package(TIFF)
include_directories(${TIFF_INCLUDE_DIR})

###########################################################################
# Compiler/linker stuffs
###########################################################################
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("Configuring for build type 'Release'")
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(
    -msse2
    -std=c++0x
    -Wall
    -DNDEBUG
    -D__GXX_EXPERIMENTAL_CXX0X__ 
    -funroll-loops
    -mfpmath=sse
    -ftree-vectorize
    -DEIGEN_FFTW_DEFAULT
    -fopenmp
    -pthread
    -Wno-ignored-attributes # ignore annoying warnings caused by Eigen library
    -Wno-misleading-indentation # ignore annoying warnings caused by Eigen library
    -Wno-deprecated-declarations # ignore annoying warnings caused by Eigen library
    )
    set(CMAKE_CXX_FLAGS_RELEASE "-O2") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
  endif()
  message("Configuring for build type 'Release'")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Configuring for build type 'Debug'")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
else()
  message("Warning: build type ${CMAKE_BUILD_TYPE} is unrecognized")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    add_definitions(-std=c++11)
endif()


# Disable auto-linking to allow dynamic linking with MSVC
if(WIN32 AND MSVC)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNSXTOOL_EXPORT)
    add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB)
endif()


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_definitions(-std=c++11)
  #  -stdlib=libc++ 

endif()

###########################################################################
# Now add in all the components
###########################################################################

# Build the config.h on the fly
if (UNIX)
    set(RESOURCES_DIR ${CMAKE_INSTALL_PREFIX}/share/nsxtool)
elseif (WIN32)
    set(RESOURCES_DIR "$ENV{APPDATA}\\nsxtool")
    # take care of pesky backslashes in windows path names
    STRING(REGEX REPLACE "\\\\" "\\\\\\\\" RESOURCES_DIR ${RESOURCES_DIR})
endif()
configure_file(${CMAKE_SOURCE_DIR}/cmake/Config.h.in ${CMAKE_BINARY_DIR}/include/Config.h)

include_directories(${CMAKE_BINARY_DIR}/include/
                    chemistry/include
                    data/include
                    geometry/include
                    kernel/include
                    imaging/include
                    integration/include
                    materials/include
                    utils/include
                    crystal/include
                    instrument/include
                    externals/c-blosc/blosc)

file(GLOB_RECURSE sources chemistry/src/*.cpp
  data/src/*.cpp
  data/src/*.c
                          geometry/src/*.cpp
                          kernel/src/*.cpp
                          imaging/src/*.cpp
                          integration/src/*.cpp
                          materials/*.cpp
                          utils/src/*.cpp
                          crystal/src/*.cpp
                          instrument/src/*.cpp
                          )

file(GLOB_RECURSE headers chemistry/include/*.h
                          data/include/*.h
                          geometry/include/*.h
                          kernel/include/*.h
                          imaging/include/*.h
                          integration/include/*.h
                          materials/*.cpp
                          utils/include/*.h
                          crystal/include/*.h
                          instrument/include/*.h
                  )                   

# Creating the library
# Under Windows and Miscrosoft Visual Studio the library will be static to avoid
# the DLL_EXPORT mess
if(MSVC)
    add_library(NSXTool STATIC ${sources} ${headers})
else()
    add_library(NSXTool SHARED ${sources} ${headers})
endif()

find_path(FFTW_INCLUDE_DIR fftw3.h)
include_directories(${FFTW_INCLUDE_DIR})

find_library(FFTW3_LIBRARIES NAMES fftw3 libfftw3 fftw3-3 libfftw3-3)




# Link
#target_link_libraries(NSXTool blosc_shared ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${FFTW3_LIBRARIES} ${HDF5_LIBRARIES} ${TIFF_LIBRARY})
target_link_libraries(NSXTool ${HDF5_LIBRARIES} blosc_shared ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${FFTW3_LIBRARIES} ${TIFF_LIBRARY})

if (BUILD_QTAPPS)
	add_subdirectory(apps)
endif()

# Enable unit testing with ctest. This must be called before any calls to add_subdirectory 
enable_testing()
# Directory with all the unit-tests. 
add_subdirectory(tests)

###########################################################################
# Installation settings
###########################################################################

# Install section, installing libraries 
install(TARGETS NSXTool DESTINATION lib)
set(INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/NSXTool/")
file(GLOB_RECURSE all_headers */include/*.h)
install(CODE "MESSAGE(\"Installing headers...\")")
install(FILES ${all_headers} DESTINATION ${INCLUDE_DIR})

# Install the databases file
install(DIRECTORY resources/databases 
        DESTINATION ${RESOURCES_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

install(DIRECTORY resources/instruments 
        DESTINATION ${RESOURCES_DIR} 
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

###########################################################################
# CPack settings
###########################################################################
        
include(CPackCommon)
    
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

    include(CPackLinuxSetup)
    message(STATUS "CPACK_PACKAGE_FILE_NAME = ${CPACK_PACKAGE_FILE_NAME}")
      
	if ("${UNIX_DIST}" MATCHES "Ubuntu")
	    string(REPLACE "," ", " CPACK_DEBIAN_PACKAGE_DEPENDS
	    "libqt5core5a,"
	    "libqt5printsupport5,"
	    "libqt5widgets5,"
	    "libqt5gui5,"
	    "libboost-filesystem1.54.0,"
	    "libboost-date-time1.54.0,"
	    "libboost-system1.54.0,"
	    "libboost-test1.54.0,"
	    "libeigen3-dev,"
	    "libfftw3-bin,"
	    "libtiff5,"
	    "libhdf5-7 (>=1.8.11)")
	endif()

endif()

set(CMAKE_INSTALL_RPATH "/usr/local/lib")

include(CPack)
