#---------------------------------------------------------------------------------------------------
#  CheckCompiler.cmake
#---------------------------------------------------------------------------------------------------
enable_language(CXX)
enable_language(C)

# determine if compiler is GNU/clang variety
if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(COMPILER_IS_GNU TRUE)
  set(COMPILER_IS_CLANG FALSE)
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  set(COMPILER_IS_GNU_OR_CLANG TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(COMPILER_IS_GNU_OR_CLANG TRUE)
  set(COMPILER_IS_GNU FALSE)
  set(COMPILER_IS_CLANG TRUE)
else()
    set(COMPILER_IS_GNU_OR_CLANG FALSE)
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

# check whether the compiler is recognized
if(COMPILER_IS_GNU_OR_CLANG)
  elseif(COMPILER_IS_MSVC)
else()
  message(WARNING "C++ compiler not recognized; build may fail")
endif()

# enable c++11 support
if (CMAKE_VERSION VERSION_LESS "3.1")
    if(COMPILER_IS_GNU_OR_CLANG)
        add_compile_options(-std=c++11)
    elseif(COMPILER_IS_MSVC)
        # nothing to do...? does MSVC support different standards?
    endif()
else()
  set(CMAKE_CXX_STANDARD 11)
endif()

# disable annoying warnings during msvc build
if (COMPILER_IS_MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4348 /wd4127 /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4348 /wd4127 /MP")
endif()

# special configuration for GNU/clang
if(COMPILER_IS_GNU_OR_CLANG)
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-pthread)    
    add_definitions(-DEIGEN_FFTW_DEFAULT)
    add_definitions(-D_USE_MATH_DEFINES)
  elseif(COMPILER_IS_MSVC)
    add_definitions(/D_USE_MATH_DEFINES)
    add_compile_options(/W2) # reasonable warning level
    add_definitions(/DH5_BUILT_AS_DYNAMIC_LIB)
    add_definitions(/DBOOST_ALL_NO_LIB)
    add_definitions(/D_USE_MATH_DEFINES)
    add_definitions(/DNSXTOOL_EXPORT)
endif()

# clang specific
if(COMPILER_IS_CLANG)
  add_compile_options(-Werror=delete-incomplete)
endif()

# gcc specific
if(COMPILER_IS_GNU)
  if (GCC_VERSION VERSION_GREATER 4.9)
    add_compile_options(-Werror=delete-incomplete)
    if (BUILD_OPTIMIZED_DEBUG)
      add_compile_options(-Og)
    endif()
  endif()
endif()


# optional optimized debug for GCC
if(BUILD_OPTIMIZED_DEBUG AND NOT COMPILER_IS_GNU)
  message(WARNING "BUILD_OPTIMIZED_DEBUG=ON has no effect on builds with ${CMAKE_CXX_COMPILER_ID}")
endif()

# code sanitizer
if (NSX_SANITIZE)
  message("The sanitize options are: thread, memory, undefined, dataflow, cfi, safe-stack")
  message(STATUS "The sanitize option '${NSX_SANITIZER}' has been selected")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
endif()

message("Finished configuring comspiler")
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
