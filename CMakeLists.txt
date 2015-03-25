cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/externals/c-blosc/cmake)

project(NSXTOOL_PROJECT)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

#Set package name here
set(CPACK_PACKAGE_NAME "nsxtool")

###########################################################################
# Check dependencies
###########################################################################

###### Find boost libraries  ##############
find_package(Boost 1.54.0 COMPONENTS date_time system filesystem unit_test_framework python REQUIRED)
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
find_package(Blosc)

###### TIFF library
find_package(TIFF)

#### Internal dependencies
add_subdirectory(externals)

###########################################################################
# Compiler/linker stuffs
###########################################################################

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
    )
    set(CMAKE_CXX_FLAGS_RELEASE "-O2") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    add_definitions(-std=c++11)
endif()


# Disable auto-linking to allow dynamic linking with MSVC
if(WIN32 AND MSVC)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNSXTOOL_EXPORT)
endif()

###########################################################################
# Now add in all the components
###########################################################################

include_directories(chemistry/include
                    data/include
                    geometry/include
                    kernel/include
                    integration/include
                    materials/include
                    utils/include
                    crystal/include
                    instrument/include)

file(GLOB_RECURSE sources chemistry/src/*.cpp
                          data/src/*.cpp
                          geometry/src/*.cpp
                          kernel/src/*.cpp
                          integration/src/*.cpp
                          materials/*.cpp
                          utils/src/*.cpp
                          crystal/src/*.cpp
                          instrument/src/*.cpp
                  )                   

# Creating the library
# Under Windows and Miscrosoft Visual Studio the library will be static to avoid
# the DLL_EXPORT mess
if(MSVC)
    add_library(NSXTool STATIC ${sources})
else()
    add_library(NSXTool SHARED ${sources})
endif()

find_library(FFTW3_LIBRARIES NAMES fftw3 libfftw3)
 
# Link
target_link_libraries(NSXTool ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${FFTW3_LIBRARIES} ${HDF5_LIBRARIES} ${BLOSC_LIBRARY} ${TIFF_LIBRARY})

# Enable unit testing with ctest. This must be called before any calls to add_subdirectory 
enable_testing()
# Directory with all the unit-tests. 
add_subdirectory(tests)

###########################################################################
# Installation settings
###########################################################################

# Install section, installing libraries 
install(TARGETS NSXTool DESTINATION NSXTool)

if(UNIX)
	set(INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/NSXTool/")
	file(GLOB_RECURSE all_headers */include/*.h)
	install(CODE "MESSAGE(\"Installing headers...\")")
	install(FILES ${all_headers} DESTINATION ${INCLUDE_DIR})
endif()

install(DIRECTORY resources/databases 
        DESTINATION $ENV{HOME}/.nsxtool 
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN "CVS" EXCLUDE 
        PATTERN ".svn" EXCLUDE)

###########################################################################
# CPack settings
###########################################################################
        
 # Keep this in a defaulted-to-off option
set(ENABLE_CPACK CACHE BOOL "Switch to enable CPack package generation")
if (ENABLE_CPACK)

    include(CPackCommon)

    # Linux packaging
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        include(CPackLinuxSetup)
        # let people know what is coming out the other end
        message(STATUS "CPACK_PACKAGE_FILE_NAME = ${CPACK_PACKAGE_FILE_NAME}")
    endif()

    set(BOOST_MIN_VERSION 1.54.0)      

    # rhel requirements
    set(CPACK_RPM_PACKAGE_REQUIRES "boost >= ${BOOST_MIN_VERSION}")
    set(CPACK_RPM_PACKAGE_REQUIRES "fftw3")
    set(CPACK_RPM_PACKAGE_REQUIRES "eigen3")
    set(CPACK_RPM_PACKAGE_REQUIRES "hdf5")
    set(CPACK_RPM_PACKAGE_REQUIRES "tiff")
    set(CPACK_RPM_PACKAGE_REQUIRES "python")
    set(CPACK_RPM_PACKAGE_REQUIRES "numpy")
      
    if ("${UNIX_DIST}" MATCHES "Ubuntu")
        string(REPLACE "," ", " CPACK_DEBIAN_PACKAGE_DEPENDS
        "libboost-filesystem-dev,"
        "libboost-date-time-dev,"
        "libboost-python-dev,"
        "libboost-system-dev,"
        "libboost-test-dev,"
        "libeigen3-dev,"
        "libfftw3-dev,"
        "libtiff-dev,"
        "libhdf5-serial-dev,"
        "python,"
        "python-numpy")
    endif()
    
    include(CPack)
    
endif()
