#---------------------------------------------------------------------------------------------------
#  CheckCompiler.cmake
#---------------------------------------------------------------------------------------------------
enable_language(CXX)
enable_language(C)

set(CMAKE_CXX_STANDARD 17)

# determine if compiler is GNU/clang variety
if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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

# special configuration for GNU/clang
if(COMPILER_IS_GNU_OR_CLANG)
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-sign-compare)
    add_compile_options(-pthread)
    add_definitions(-DEIGEN_FFTW_DEFAULT)
    add_definitions(-D_USE_MATH_DEFINES)
elseif(COMPILER_IS_MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4348 /wd4127 /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4348 /wd4127 /MP")
    add_definitions(/D_USE_MATH_DEFINES)
    add_compile_options(/W2) # reasonable warning level
    add_definitions(/DH5_BUILT_AS_DYNAMIC_LIB)
    add_definitions(/DBOOST_ALL_NO_LIB)
    add_definitions(/D_USE_MATH_DEFINES)
    add_definitions(/DNSXTOOL_EXPORT)
endif()

# clang specific
if(COMPILER_IS_CLANG)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-incomplete>)
endif()

# gcc specific
if(COMPILER_IS_GNU)
    if(GCC_VERSION VERSION_GREATER 4.9)
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-incomplete>)
        if(BUILD_OPTIMIZED_DEBUG)
            add_compile_options(-Og)
        endif()
    endif()
endif()

# code sanitizer
if(NSX_SANITIZE)
    message("The sanitize options are: thread, memory, undefined, dataflow, cfi, safe-stack")
    message(STATUS "The sanitize option '${NSX_SANITIZER}' has been selected")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
endif()

message("Finished configuring compiler")
