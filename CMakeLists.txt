cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/macros)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/externals/c-blosc/cmake)

project(NSXTOOL_PROJECT)

###########################################################################
# Options
###########################################################################

option(BUILD_QTAPPS "Build Qt based application." ON)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)
option(BUILD_COVERAGE_REPORT "Build code coverage report" OFF)
option(BUILD_WITH_OPENMP "Build with OpenMP support" OFF)
option(BUILD_GSL "Build GSL external dependency" OFF)
option(NSX_BUILD_STATIC "Link nsxlib statically" OFF)
option(NSX_VALGRIND "Run tests with valgrind" OFF)
option(NSX_SANITIZE "Compile with clang's -fsanitize (must also set NSX_SANITIZER variable)" OFF)
option(NSX_TIDY "Add clang-tidy custom target" OFF)
option(NSX_PYTHON "Generate Python bindings" ON)
option(NSX_PYTHON3 "Force Python 3" ON)
option(NSX_DOCS "Build documentation" OFF)

###########################################################################
# Scripts controlling build setup and dependencies
###########################################################################
include(GeneratePythonDocs)
include(GetSubdirectories)
include(CMakeFindBinUtils)
include(BuildType)             # build types: debug and release
include(CheckDependencies)     # checkexternal dependencies
include(CheckCompiler)         # configure compiler

if(NSX_TIDY)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  include(ClangTidy)
endif()

if(BUILD_COVERAGE_REPORT)
  include(CheckCoverage)        # code coverage
endif()

if(NSX_DOCS)
  include(Doxygen)
endif()

###########################################################################
# Now add in all the components
###########################################################################

# Build the config.h on the fly
if (UNIX)
  set(APPLICATION_DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/nsxtool)
elseif (WIN32)
  set(APPLICATION_DATA_PATH ${CMAKE_INSTALL_PREFIX}\\share\\nsxtool)
  # take care of pesky backslashes in windows path names
  STRING(REGEX REPLACE "\\\\" "\\\\\\\\" APPLICATION_DATA_PATH ${APPLICATION_DATA_PATH})
endif()
configure_file(${CMAKE_SOURCE_DIR}/cmake/NSXConfig.h.in ${CMAKE_BINARY_DIR}/include/nsxlib/NSXConfig.h)

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

if (BUILD_QTAPPS)
    add_subdirectory(apps)
endif()

# additional tools
add_subdirectory(tools)

# Enable unit testing with ctest. This must be called before any calls to add_subdirectory
enable_testing()
# Directory with all the unit-tests.
add_subdirectory(tests)

# swig + python tests
if(NSX_PYTHON)
  add_subdirectory(swig)
endif()


# code coverage report
if(BUILD_COVERAGE_REPORT)
  set(ignore_dirs "'/usr/*'")
  #list(APPEND ignore_dirs "'*/tests/*'")
  add_coverage_target(coverage "${ignore_dirs}")
endif()


###########################################################################
# Installation settings
###########################################################################

# Install the databases file
install(DIRECTORY resources/databases
        DESTINATION share/nsxtool
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

install(DIRECTORY resources/instruments
        DESTINATION share/nsxtool
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

###########################################################################
# CPack settings
###########################################################################

include(CPackSetup)


