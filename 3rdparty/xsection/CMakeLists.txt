cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

project(XSection VERSION 0.1 LANGUAGES CXX)

###########################################################################
# Options
###########################################################################

if(NOT DEFINED BUILD_SHARED_LIBS)
    option(BUILD_SHARED_LIBS "Build as shared library" ON)
endif()

###########################################################################
# Compiler settings
###########################################################################
set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    set(CTEST_CONFIGURATION_TYPE "${JOB_BUILD_CONFIGURATION}")
    if(BUILD_SHARED_LIBS)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    option(PEDANTIC "Compile with pedantic warnings" ON)
    option(WERROR "Treat warnings as errors" OFF)
    message("Note for maintainer: replace overlong string literal") # TODO
    add_compile_options(-Wno-overlength-strings)
    if(PEDANTIC)
        add_compile_options(-pedantic -Wall)
    endif()
    if(WERROR)
        add_compile_options(-Werror)
    endif()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${DEB_FLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -w")
endif()

###########################################################################
# Dependences
###########################################################################

set(Boost_USE_MULTITHREADED ON)
# We need the Boost libraries fusion, phoenix, and spirit.
# They all are header only.
# Therefore the following find_package command goes without "COMPONENTS".
find_package(Boost 1.53.0 REQUIRED)
if(Boost_FOUND)
    include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
    message(STATUS "Found boost:")
    message(STATUS "  version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}")
    message(STATUS "  libraries: ${Boost_LIBRARIES}")
    message(STATUS "  headers: ${Boost_INCLUDE_DIRS}")
elseif(NOT Boost_FOUND)
    message(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
endif()

find_package(YAMLCPP REQUIRED)

include(CTest)

###########################################################################
# Finalize
###########################################################################

set(destination ${CMAKE_INSTALL_PREFIX})

add_subdirectory(lib)
add_subdirectory(test)
