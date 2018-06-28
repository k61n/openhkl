# Find the FFTW includes and library.
# 
# This module defines
# FFTW_INCLUDE_DIR, where to locate fftw3.h file
# FFTW_LIBRARIES, the libraries to link against to use fftw3
# FFTW_FOUND.  If false, you cannot build anything that requires fftw3.
# FFTW_LIBRARY, where to find the libfftw3 library.

set(FFTW_FOUND 0)
if(FFTW_LIBRARY AND FFTW_INCLUDE_DIR)
  set(FFTW_FIND_QUIETLY TRUE)
endif()

#if(NOT WIN32)
if(TRUE)
    find_path(FFTW_INCLUDE_DIR fftw3.h
      $ENV{FFTW_DIR}
      $ENV{FFTW_DIR}/include
        $ENV{FFTW3} $ENV{FFTW3}/include $ENV{FFTW3}/api
        /usr/local/include
        /usr/include
        /opt/fftw3/include
        /opt/local/include
        DOC "Specify the directory containing fftw3.h"
    )

    find_library(FFTW_LIBRARY NAMES fftw3 libfftw3 fftw3-3 libfftw3-3 PATHS
      $ENV{FFTW_DIR}
      $ENV{FFTW_DIR}/lib
        $ENV{FFTW3} $ENV{FFTW3}/lib $ENV{FFTW3}/.libs
        /usr/local/lib
        /usr/lib 
        /opt/fftw3/lib
        HINTS /opt/local/lib
        DOC "Specify the fftw3 library here."
    )
else()
    find_path(FFTW_INCLUDE_DIR fftw3.h
        $ENV{FFTW_DIR}/include
        "C:/opt/local/include"
    )

    find_library(FFTW_LIBRARY NAMES libfftw3-3 PATHS
        $ENV{FFTW_DIR}/lib
        $ENV{FFTW3} $ENV{FFTW3}/lib $ENV{FFTW3}/.libs
        "C:/opt/local/lib"
    )
    
endif()

if(FFTW_INCLUDE_DIR AND FFTW_LIBRARY)
  set(FFTW_FOUND 1 )
endif()


if(NOT FFTW_FOUND)
    if(FFTW_FIND_REQUIRED)
        message( FATAL_ERROR "FindFFTW: can't find fftw3 header or library: include dir is ${FFTW_INCLUDE_DIR} and library is ${FFTW_LIBRARY}" )
    endif()
endif()

message(STATUS "Found FFTW")
message(STATUS "  libraries: ${FFTW_LIBRARY}")
message(STATUS "  headers: ${FFTW_INCLUDE_DIR}")

set(FFTW_LIBRARIES ${FFTW_LIBRARY})

mark_as_advanced(FFTW_FOUND FFTW_LIBRARY FFTW_INCLUDE_DIR)
