cmake_minimum_required(VERSION 3.6 FATAL_ERROR) # 3.6+ required by IFW for VS2015+

if(POLICY CMP0078)
  cmake_policy(SET CMP0078 NEW)
endif()

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/macros)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

project(nsxtool VERSION 0.1.0)

###########################################################################
# Options
###########################################################################

option(BUILD_NSX_APPS "Build nsx based application." ON)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)
option(BUILD_COVERAGE_REPORT "Build code coverage report" OFF)
option(BUILD_WITH_OPENMP "Build with OpenMP support" OFF)
option(NSX_BUILD_STATIC "Link core statically" OFF)
option(NSX_VALGRIND "Run tests with valgrind" OFF)
option(ENABLE_CPACK "Enable CPack package generation" OFF)
option(NSX_SANITIZE "Compile with clang's -fsanitize (must also set NSX_SANITIZER variable)" OFF)
option(NSX_TIDY "Add clang-tidy custom target" OFF)
option(NSX_PYTHON "Generate Python bindings" ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)

###########################################################################
# Scripts controlling build setup and dependencies
###########################################################################

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()
set(NSX_LIBRARY nsx)
set(NSXQT_EXECUTABLE nsxqt)

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
include(SetLinkerFlags)
include(ConfigureInstall)

if(NSX_TIDY)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    include(ClangTidy)
endif()

if(BUILD_COVERAGE_REPORT)
    include(CheckCoverage)
endif()


# Enable unit testing with ctest. This must be called before any calls to add_subdirectory
enable_testing()

add_subdirectory(core)

if (BUILD_NSX_APPS)
    find_package(QCR MODULE REQUIRED)
    message(STATUS "libQCR: FOUND=${QCR_FOUND}, VERSION=${QCR_VERSION}, LIB=${QCR_LIBRARIES}")
    include(SetupQt)
    add_subdirectory(3rdparty/QCustomPlot)
    add_subdirectory(apps)
    add_subdirectory(nsxgui)
endif()

# additional tools
# add_subdirectory(tools)

# Directory with all the unit-tests.
add_subdirectory(test)

# swig + python tests
if(NSX_PYTHON)
    add_subdirectory(swig)
endif()


# code coverage report
if(BUILD_COVERAGE_REPORT)
    list(APPEND ignore_dirs "'/usr/include/*'")
    list(APPEND ignore_dirs "'/usr/lib/*'")
    list(APPEND ignore_dirs "'${CMAKE_BINARY_DIR}/*'")
    list(APPEND ignore_dirs "'${CMAKE_SOURCE_DIR}/externals/*'")

    message("coverage ignore dirs = ${ignore_dirs}")

    #list(APPEND ignore_dirs "'*/tests/*'")
    add_coverage_target(coverage "${ignore_dirs}")
endif()


###########################################################################
# CPack settings
###########################################################################

if(ENABLE_CPACK)
    include(CPackSetup)
endif()
