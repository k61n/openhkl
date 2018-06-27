cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/macros)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/externals/c-blosc/cmake)

project(nsxtool)

###########################################################################
# Options
###########################################################################

option(BUILD_NSX_APPS "Build nsx based application." ON)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)
option(BUILD_COVERAGE_REPORT "Build code coverage report" OFF)
option(BUILD_WITH_OPENMP "Build with OpenMP support" OFF)
option(BUILD_GSL "Build GSL external dependency" OFF)
option(NSX_BUILD_STATIC "Link nsxlib statically" OFF)
option(NSX_VALGRIND "Run tests with valgrind" OFF)
option(ENABLE_CPACK "Enable CPack package generation" OFF)
option(NSX_SANITIZE "Compile with clang's -fsanitize (must also set NSX_SANITIZER variable)" OFF)
option(NSX_TIDY "Add clang-tidy custom target" OFF)
option(NSX_PYTHON "Generate Python bindings" ON)
option(USE_PYTHON3 "Force Python 3" ON)
option(NSX_DOCS "Build documentation" OFF)

###########################################################################
# Scripts controlling build setup and dependencies
###########################################################################

include(Version)
include(NSXToolConfiguration)
include(ConfigureInstall)
include(GeneratePythonDocs)
include(GetSubdirectories)
include(CMakeFindBinUtils)
include(BuildType)
include(CheckDependencies)
include(SetCompilerFlags)
include(SetLinkerFlags)

if(NSX_TIDY)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  include(ClangTidy)
endif()

if(BUILD_COVERAGE_REPORT)
  include(CheckCoverage)
endif()

if(NSX_DOCS)
  include(Doxygen)
endif()


# Enable unit testing with ctest. This must be called before any calls to add_subdirectory
enable_testing()
  
###########################################################################
# RPATH manipulation
###########################################################################

# For info use 'objdump -x nsxqt | grep RPATH' to the installed executable (not the built one)
# to check that the RPATH is correctly set

# Update the RPATH with the path of some dependencies
list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib ${GSL_LIBRARIES})

# Use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# When building, don't use the install RPATH already (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# Disable the RPATH stripping during installation
# See here for more explanations: https://stackoverflow.com/questions/32469953/why-is-cmake-designed-so-that-it-removes-runtime-path-when-installing
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_subdirectory(nsxlib)

if (BUILD_NSX_APPS)
    add_subdirectory(apps)
endif()

# additional tools
add_subdirectory(tools)

# Directory with all the unit-tests.
add_subdirectory(tests)

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

