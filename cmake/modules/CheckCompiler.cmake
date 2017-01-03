#---------------------------------------------------------------------------------------------------
#  CheckCompiler.cmake
#---------------------------------------------------------------------------------------------------
enable_language(CXX)
enable_language(C)

# determine if compiler is GNU/clang variety
if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_IS_GNU_OR_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

  set(COMPILER_IS_GNU_OR_CLANG TRUE)
  message(">>>>>>>>>>>>>>>>>>>>>> the compiler is clang")
  message(">>>>>>>>>>>>>>>>>>>>>> cxx flags: ${CMAKE_CXX_FLAGS}; win32: ${WIN32}; msvc: ${MSVC}; compiler exe ${CMAKE_CXX_COMPILER}")
else()
    set(COMPILER_IS_GNU_OR_CLANG FALSE)
endif()

# try to fix problem with msvc+llvm
if(WIN32 AND MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Building with MSVC+Clang on Windows")
endif()

# check whether compiler is MSVC
if(DEFINED MSVC)
  set(COMPILER_IS_MSVC MSVC)
else()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(COMPILER_IS_MSVC TRUE)
  else()
    set(COMPILER_IS_MSVC FALSE)
  endif()
endif()

# disable annoying warnings during msvc build
if (COMPILER_IS_MSVC)
    # annoying warning triggered by boost::spirit
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4348")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4348")
endif()

# enable c++11 support
if (CMAKE_VERSION VERSION_LESS "3.1")
    if(COMPILER_IS_GNU_OR_CLANG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(COMPILER_IS_MSVC)
        # todo...?
    endif()
else()
  set(CMAKE_CXX_STANDARD 11)
endif()

# check whether the compiler is recognized
if(COMPILER_IS_GNU_OR_CLANG)
elseif(COMPILER_IS_MSVC)
else()
  message(WARNING "C++ compiler not recognized")
endif()

# build with debugging information
if ( BUILD_WITH_DEBUG_INFO )
    if ( COMPILER_IS_GNU_OR_CLANG)
        add_definitions(-g)
    elseif (COMPILER_IS_MSVC)
        add_definitions(/DEBUG)
    endif()
endif( BUILD_WITH_DEBUG_INFO )

# special configuration for GNU/clang
if(COMPILER_IS_GNU_OR_CLANG)
    add_definitions(-Wall)
    add_definitions(-pthread)
    add_definitions(-DEIGEN_FFTW_DEFAULT)
    add_definitions(-D_USE_MATH_DEFINES)
    message("TESTING: COMPILER IS CLANG OR GNU")
  elseif(COMPILER_IS_MSVC)
    add_definitions(/D_USE_MATH_DEFINES)
endif()

 

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(
        #-Wall
        #-Wno-ignored-attributes # ignore annoying warnings caused by Eigen library
        #-Wno-misleading-indentation # ignore annoying warnings caused by Eigen library
        #-Wno-deprecated-declarations # ignore annoying warnings caused by Eigen library
        #-fopenmp
        #-pthread
        #-DEIGEN_FFTW_DEFAULT
    )
endif()

if(COMPILER_IS_MSVC)
    add_definitions(/DH5_BUILT_AS_DYNAMIC_LIB)
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

# ensure that NDEBUG is defined for release builds
#if(CMAKE_BUILD_TYPE STREQUAL "Release")
#    if ( COMPILER_IS_GNU_OR_CLANG)
#        add_definitions(-DNDEBUG)
#    elseif(MSVC)
#        add_definitions(/DNDEBUG)
#    else()
#        message(WARNING "NDEBUG should be defined but may not be on compiler ${CMAKE_COMPILER_ID}")
#    endif()
#endif()


# Disable auto-linking to allow dynamic linking with MSVC
if(WIN32 AND COMPILER_IS_MSVC)
    add_definitions(/DBOOST_ALL_NO_LIB)
    add_definitions(/D_USE_MATH_DEFINES)
    add_definitions(/DNSXTOOL_EXPORT)
    add_definitions(/DH5_BUILT_AS_DYNAMIC_LIB)
endif()


# optional optimization for debug build
if(BUILD_OPTIMIZED_DEBUG)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O1")
    else()
        message(WARNING "BUILD_OPTIMIZED_DEBUG=ON has no effect on builds with ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

message("Finished configuring compiler")
message(STATUS "The c++ compiler is ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "The build type is ${CMAKE_BUILD_TYPE}")

if ( CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "The build flags are ${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(STATUS "The build flags are ${CMAKE_CXX_FLAGS_MINSIZEREL} ${CMAKE_CXX_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "The build flags are ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CMAKE_CXX_FLAGS}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "The build flags are ${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
else()
    message(WARNING "Build type is not recognized")
endif()
