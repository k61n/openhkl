cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

# Add some paths to the cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/externals/c-blosc/cmake)

project(NSXTOOL_PROJECT)

set(NSXTOOL_VERSION_MAJOR 1)
set(NSXTOOL_VERSION_MINOR 0)

###########################################################################
# Options
###########################################################################

option(BUILD_QTAPPS "Build Qt based application." ON)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)
option(BUILD_COVERAGE_REPORT "Build code coverage report" OFF)
option(BUILD_WITH_OPENMP "Build with OpenMP support" OFF)
option(BUILD_DEBIAN "Build debian .deb packages" OFF)
option(BUILD_GSL "Build GSL external dependency" OFF)
option(NSX_BUILD_STATIC "Link nsxlib statically" OFF)
option(NSX_VALGRIND "Run tests with valgrind" OFF)
option(NSX_SANITIZE "Compile with clang's -fsanitize (must also set NSX_SANITIZER variable)" OFF)
option(NSX_TIDY "Add clang-tidy custom target" OFF)
option(NSX_PYTHON "Generate Python bindings" ON)
option(NSX_PYTHON3 "Force Python 3" ON)
option(NSX_DOCS "Build documentation" ON)

###########################################################################
# Scripts controlling build setup and dependencies
###########################################################################
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

#Set package name here
set(CPACK_PACKAGE_NAME "nsxtool")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neutron Single Crystal Diffraction Data Reduction")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "${NSXTOOL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${NSXTOOL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;" )

###########################################################################
# Now add in all the components
###########################################################################

# Build the config.h on the fly
if (UNIX)
  set(APPLICATION_DATA_PATH ${CMAKE_INSTALL_PREFIX}/share/nsxtool)
elseif (WIN32)
  set(APPLICATION_DATA_PATH "$ENV{APPDATA}\\nsxtool")
  # take care of pesky backslashes in windows path names
  STRING(REGEX REPLACE "\\\\" "\\\\\\\\" APPLICATION_DATA_PATH ${APPLICATION_DATA_PATH})
endif()
configure_file(${CMAKE_SOURCE_DIR}/cmake/NSXConfig.h.in ${CMAKE_BINARY_DIR}/include/NSXConfig.h)

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

# Install section, installing libraries
#install(TARGETS NSXTool DESTINATION lib)
#set(INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/NSXTool/")
#install(CODE "MESSAGE(\"Installing headers...\")")
#install(FILES ${headers} DESTINATION ${INCLUDE_DIR})

# Install the databases file
install(DIRECTORY resources/databases
        DESTINATION ${APPLICATION_DATA_PATH}
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

install(DIRECTORY resources/instruments
        DESTINATION ${APPLICATION_DATA_PATH}
        FILE_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_WRITE GROUP_READ GROUP_EXECUTE WORLD_WRITE WORLD_READ WORLD_EXECUTE
        PATTERN ".svn" EXCLUDE)

###########################################################################
# CPack settings
###########################################################################

include(CPackCommon)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${BUILD_DEBIAN})
  include(CPackLinuxSetup)
  message(STATUS "CPACK_PACKAGE_FILE_NAME = ${CPACK_PACKAGE_FILE_NAME}")
  if ("${UNIX_DIST}" MATCHES "Ubuntu" OR ${BUILD_DEBIAN})
    # automatically generate dependencies
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  endif()
endif()

set(CMAKE_INSTALL_RPATH "/usr/local/lib")
include(CPack)
