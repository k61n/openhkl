# Steca top-level configuration

cmake_minimum_required(VERSION 3.6 FATAL_ERROR) # 3.6+ required by IFW for VS2015+
cmake_policy(VERSION 3.6)
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW) # since 3.13
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

project(NsxGui)
message(STATUS "Building NsxGui")

set(CMAKE_PROJECT_DESCRIPTION # for display in installer
    "NsxGui")

option(BUILD_FALLBACK "Build and use the fallback 3rdparty libraries" OFF)
option(COVERAGE "Support code coverage report" OFF)

set(LIB_MAN     OFF CACHE INTERNAL "" FORCE)
set(LIB_INSTALL OFF CACHE INTERNAL "" FORCE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__STRICT_ANSI__ -DQT_LOGGING_RULES=\"*.debug=true\"")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}")
    set(MS_NOWARN "/wd4018 /wd4068 /wd4101 /wd4244 /wd4267 /wd4305 /wd4715 /wd4996")
    set(MS_D "-D_CRT_SECURE_NO_WARNINGS -D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${MS_NOWARN} ${MS_D}")
    set(CTEST_CONFIGURATION_TYPE "${JOB_BUILD_CONFIGURATION}")
    set(DEFAULT_BUILD_SHARED_LIBS ON)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/$<CONFIG>)
else()
    if(Clang)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fshow-overloads=best")
    endif()
    # -fsanitize=address -fsanitize=undefined
    set(DEB_FLAGS "-Wall -Wpedantic -Wno-sign-compare -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${DEB_FLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -w")
    set(DEFAULT_BUILD_SHARED_LIBS ON)
endif()
set(BUILD_SHARED_LIBS ${DEFAULT_BUILD_SHARED_LIBS})

if(COVERAGE)
    include(CheckCoverage)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -coverage")
endif()

foreach(comp Qt5Core Qt5Gui Qt5Widgets Qt5Network)
    find_package(${comp} REQUIRED)
    message(STATUS "found ${comp}: version=${${comp}_VERSION_STRING} dir=${${comp}_DIR} lib=${${comp}_LIBRARIES}")
endforeach()

# how to build 3rd party libraries:
set(LIB_MAN OFF)
set(LIB_INSTALL OFF)

#configure_file("manifest.h.in" "${CMAKE_BINARY_DIR}/manifest.h")

add_subdirectory(qcr)
add_subdirectory(gui)
add_subdirectory(main)

message(STATUS "NsxGui components all configured")

# code coverage report
if(COVERAGE)
    list(APPEND ignore_dirs "'/usr/include/*'")
    list(APPEND ignore_dirs "'/usr/lib/*'")
    list(APPEND ignore_dirs "'3rdparty/*'")
    list(APPEND ignore_dirs "'${CMAKE_BINARY_DIR}/*'")
    add_coverage_target(coverage "${ignore_dirs}")
endif()
