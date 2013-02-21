CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

PROJECT(NSXTOOL)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

###### Find boost libraries  ##############
FIND_PACKAGE(Boost 1.42.0 COMPONENTS date_time filesystem system REQUIRED)
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
FIND_PACKAGE(GSL)
#FIND_LIBRARY(GSL gsl)
#FIND_LIBRARY(GSL_CBLAS gslcblas)


###### Find CGAL library (used for Convex Hull triangulations #######
FIND_PACKAGE(CGAL)
#FIND_LIBRARY(CGAL_LIBRARIES CGAL)
#IF (NOT CGAL_FOUND)
#  MESSAGE(FATAL_ERROR "Unable to find CGAL libraries")
#ENDIF()

###### Find GMP 
FIND_PACKAGE(GMP)
#FIND_LIBRARY(GMP_LIBRARIES gmp)
#IF(NOT GMP_FOUND)
#  MESSAGE(FATAL_ERROR "Found gmp libraries")
#ENDIF()

###### Find MPFR 
FIND_PACKAGE (MPFR)
#FIND_LIBRARY(MPFR_LIBRARIES mpfr)
#IF(NOT MPFR_FOUND)
#    MESSAGE(FATAL_ERROR "problem")
#ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS("-Wall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
ENDIF()

# Disable auto-linking to allow dynamic linking with MSVC
IF(WIN32 AND MSVC)
    ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)
ENDIF()
    
add_subdirectory(src)