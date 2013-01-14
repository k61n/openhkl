CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

PROJECT(NSXTOOL)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "RELEASE")
ENDIF()

###### Find boost libraries  ##############
FIND_PACKAGE(Boost 1.42.0 COMPONENTS date_time filesystem REQUIRED)
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

###### Find GSL and GSL_CBLAS ######
FIND_LIBRARY(GSL gsl)
FIND_LIBRARY(GSL_CBLAS gslcblas)

###### Find CGAL library (used for Convex Hull triangulations #######
include("cmake/FindCGAL.cmake")
FIND_LIBRARY(CGAL_LIBRARIES CGAL)
IF (NOT CGAL_FOUND)
  MESSAGE(FATAL_ERROR "Unable to find CGAL libraries")
ENDIF()

###### Find GMP 
include ("cmake/FindGMP.cmake")
FIND_LIBRARY(GMP_LIBRARIES gmp)
IF(NOT GMP_FOUND)
  MESSAGE(FATAL_ERROR "Found gmp libraries")
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS("-Wall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
ENDIF()

add_subdirectory(src)