CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

PROJECT(NSXTOOL_PROJECT)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

###### Find boost libraries  ##############
FIND_PACKAGE(Boost 1.42.0 COMPONENTS date_time filesystem system unit_test_framework REQUIRED)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}")
  SET(Boost_USE_STATIC_LIBS OFF)
  SET(Boost_USE_MULTITHREADED ON)
  SET(Boost_USE_STATIC_RUNTIME OFF)
  MESSAGE(STATUS "Found boost: " ${Boost_INCLUDE_DIRS})
  MESSAGE(STATUS "Boost" ${Boost_LIBRARIES})
ELSEIF(NOT Boost_FOUND)
  MESSAGE(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
ENDIF()


###### Find OPENMP 
FIND_PACKAGE (OpenMP)

###### Find CMinpack
FIND_PACKAGE(CMinpack)

###### Find the Eigen3 
FIND_PACKAGE(Eigen3)
include_directories(${EIGEN3_INCLUDE_DIR})

###### Find Python 
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})


IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS(
    -std=c++0x
    -Wall
    -DNDEBUG
    -DBOOST_UBLAS_MOVE_SEMANTICS
    -D__GXX_EXPERIMENTAL_CXX0X__ 
)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    ADD_DEFINITIONS(-std=c++11
    )
ENDIF()

# Disable auto-linking to allow dynamic linking with MSVC
IF(WIN32 AND MSVC)
    ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)
ENDIF()

include_directories( analysis/include
                     data/include
                     geometry/include
                     integration/include
                     materials/include
                     maths/include
                     utils/include
                     crystal/include)

file(GLOB_RECURSE sources analysis/src/*.cpp
                  data/src/*.cpp
                  geometry/src/*.cpp
                  integration/src/*.cpp
                  materials/*.cpp
                  maths/src/*.cpp
                  utils/src/*.cpp
                  crystal/src/*.cpp)                   

# Creating the library
add_library(NSXTool SHARED ${sources})
 
# Link
TARGET_LINK_LIBRARIES(NSXTool ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${CMINPACK_LIBRARIES})

# Enable unit testing with ctest. This must be called before any calls to add_subdirectory 
enable_testing()
# Directory with all the unit-tests. 
add_subdirectory(tests)

# Install section, installing libraries 
INSTALL(TARGETS NSXTool DESTINATION NSXTool)

# 
IF(UNIX)
	SET(INCLUDE_DIR "/usr/local/include/NSXTool/")
	file(GLOB_RECURSE all_headers *.h)
	INSTALL(CODE "MESSAGE(\"Installing headers...\")")
	INSTALL(FILES ${all_headers}	DESTINATION ${INCLUDE_DIR})
ENDIF(UNIX)



