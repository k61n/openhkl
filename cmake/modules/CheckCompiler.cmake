#---------------------------------------------------------------------------------------------------
#  CheckCompiler.cmake
#---------------------------------------------------------------------------------------------------


# determine if compiler is GNU/clang variety
if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_IS_GNU_OR_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(COMPILER_IS_GNU_OR_CLANG TRUE)
else()
    set(COMPILER_IS_GNU_OR_CLANG FALSE)
endif()

# check whether compiler is MSVC
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(MSVC TRUE)
else()
    set(MSVC FALSE)
endif()

# enable c++11 support
if (CMAKE_VERSION VERSION_LESS "3.1")
    if(COMPILER_IS_GNU_OR_CLANG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(MSVC)
        # todo...?
    endif()
else()
  set(CMAKE_CXX_STANDARD 11)
endif()

# check whether the compiler is recognized
if(COMPILER_IS_GNU_OR_CLANG)
elseif(MSVC)
else()
  message(WARNING "C++ compiler not recognized")
endif()

# build with debugging information
if ( BUILD_WITH_DEBUG_INFO )
    if ( COMPILER_IS_GNU_OR_CLANG)
        add_definitions(-g)
    elseif (MSVC)
        add_definitions(/DEBUG)
    endif()
endif( BUILD_WITH_DEBUG_INFO )

# special configuration for GNU/clang
if(CMAKE_COMPILER_IS_GNU_OR_CLANG)
    add_definitions(-Wall)
    add_definitions(-pthread)
    add_definitions(-DEIGEN_FFTW_DEFAULT)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(
        #-Wall
        -Wno-ignored-attributes # ignore annoying warnings caused by Eigen library
        -Wno-misleading-indentation # ignore annoying warnings caused by Eigen library
        -Wno-deprecated-declarations # ignore annoying warnings caused by Eigen library
        #-fopenmp
        #-pthread
        #-DEIGEN_FFTW_DEFAULT
    )
endif()

# jmf: much of this is redundant due to cmake and gcc defaults
# we should investigate which flags actually give noticeable performance increase
#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#    if(CMAKE_COMPILER_IS_GNUCXX)
#        add_definitions(
#        -msse2
#        #-DNDEBUG
#    #-D__GXX_EXPERIMENTAL_CXX0X__
#    -funroll-loops
#    -mfpmath=sse
#    -ftree-vectorize
#    -O2
#    )
#  endif()
#  message("Configuring for build type 'Release'")
#elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
#  message("Configuring for build type 'Debug'")
#  if (CMAKE_COMPILER_IS_GNUCXX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g")
#  else()
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g")
#  endif()
#else()
#  message("Warning: build type ${CMAKE_BUILD_TYPE} is unrecognized")
#endif()


# Disable auto-linking to allow dynamic linking with MSVC
if(WIN32 AND MSVC)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DNSXTOOL_EXPORT)
    add_definitions(-DH5_BUILT_AS_DYNAMIC_LIB)
endif()

