#---------------------------------------------------------------------------------------------------
#  CheckCompiler.cmake
#---------------------------------------------------------------------------------------------------
enable_language(CXX)
enable_language(C)

set(CMAKE_CXX_STANDARD 17)

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
            message(FATAL_ERROR "Compilation with g++ ${CMAKE_CXX_COMPILER_VERSION}"
                " (older than 7.0) is not tested,"
                " and not expected to work because of insufficient support for C++17")
        endif()
        if(BUILD_OPTIMIZED_DEBUG)
            add_compile_options(-Og)
        endif()
    endif()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-sign-compare)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-incomplete>)
    add_compile_options(-pthread)
    add_definitions(-DEIGEN_FFTW_DEFAULT)
    add_definitions(-D_USE_MATH_DEFINES)
elseif(DEFINED MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4348 /wd4127 /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4348 /wd4127 /MP")
    add_compile_options(/W2) # reasonable warning level
    add_definitions(/DH5_BUILT_AS_DYNAMIC_LIB)
    add_definitions(/DBOOST_ALL_NO_LIB)
    add_definitions(/D_USE_MATH_DEFINES)
    add_definitions(/DNSXTOOL_EXPORT)
else()
    message(WARNING "Unsupported compiler, lacking compiler-specific parameterization")
endif()

# code sanitizer
if(NSX_SANITIZE)
    message("The sanitize options are: thread, memory, undefined, dataflow, cfi, safe-stack")
    message(STATUS "The sanitize option '${NSX_SANITIZER}' has been selected")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
endif()
