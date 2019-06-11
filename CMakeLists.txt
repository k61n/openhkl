cmake_minimum_required(VERSION 3.6 FATAL_ERROR) # 3.6+ required by IFW for VS2015+

if(POLICY CMP0078)
  cmake_policy(SET CMP0078 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(PreventInSourceBuilds)

project(nsxtool VERSION 0.1.0)

###########################################################################
# Options
###########################################################################

option(BUILD_NSX_GUI "Build nsx based GUI application." ON)
option(BUILD_NSX_OLD_APPS "Build old GUI." ON)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)
option(BUILD_COVERAGE_REPORT "Build code coverage report" OFF)
option(BUILD_WITH_OPENMP "Build with OpenMP support" OFF)
option(NSX_VALGRIND "Run tests with valgrind" OFF)
option(NSX_SANITIZE "Compile with clang's -fsanitize (must also set NSX_SANITIZER variable)" OFF)
option(NSX_PYTHON "Generate Python bindings" ON)

###########################################################################
# Scripts controlling build setup and dependencies
###########################################################################

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
set(NSX_LIBRARY nsx)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()

include(CheckDependencies)
include(SetCompilerFlags)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(DEFAULT_BUILD_SHARED_LIBS ON)

if(BUILD_COVERAGE_REPORT)
    include(CheckCoverage)
endif()

# Enable unit testing with ctest. This must be called before any calls to add_subdirectory
enable_testing()

add_subdirectory(base)
add_subdirectory(core)

if(BUILD_NSX_GUI)
    find_package(QCR MODULE REQUIRED)
    message(STATUS "libQCR: FOUND=${QCR_FOUND}, VERSION=${QCR_VERSION}, LIB=${QCR_LIBRARIES}")
    include(SetupQt)
    add_subdirectory(3rdparty/QCustomPlot)
    add_subdirectory(gui)
    add_subdirectory(main)
    if(BUILD_NSX_OLD_APPS)
        add_subdirectory(apps)
    endif()
endif()

add_subdirectory(test/data)
add_subdirectory(test/cpp)

if(NSX_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(test/python)
endif()

if(BUILD_COVERAGE_REPORT)
    list(APPEND ignore_dirs "'/usr/include/*'")
    list(APPEND ignore_dirs "'/usr/lib/*'")
    list(APPEND ignore_dirs "'${CMAKE_BINARY_DIR}/*'")
    list(APPEND ignore_dirs "'${CMAKE_SOURCE_DIR}/externals/*'")

    message("coverage ignore dirs = ${ignore_dirs}")

    #list(APPEND ignore_dirs "'*/tests/*'")
    add_coverage_target(coverage "${ignore_dirs}")
endif()
