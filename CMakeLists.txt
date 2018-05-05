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
# Setup resources manager helper program
###########################################################################

add_executable(make_resource ${CMAKE_SOURCE_DIR}/scripts/make_resource.cpp)
add_executable(make_resources_map ${CMAKE_SOURCE_DIR}/scripts/make_resources_map.cpp)

configure_file(${CMAKE_SOURCE_DIR}/cmake/ResourcesMap.h.in ${CMAKE_BINARY_DIR}/include/nsxlib/ResourcesMap.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/ResourcesMap.cpp.in ${CMAKE_BINARY_DIR}/src/nsxlib/ResourcesMap.cpp)

file(GLOB_RECURSE NSX_RESOURCES ${CMAKE_SOURCE_DIR}/resources/*.yml)
file(WRITE ${CMAKE_BINARY_DIR}/generate-nsx-resources.cmake 
 "foreach (res_file ${NSX_RESOURCES})
    get_filename_component(NSX_RESOURCE_NAME \${res_file} NAME_WE)

    configure_file(${CMAKE_SOURCE_DIR}/cmake/Resource.h.in ${CMAKE_BINARY_DIR}/include/nsxlib/Resource\${NSX_RESOURCE_NAME}.h)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/Resource.cpp.in ${CMAKE_BINARY_DIR}/src/nsxlib/Resource\${NSX_RESOURCE_NAME}.cpp)
    execute_process(COMMAND ${CMAKE_BINARY_DIR}/make_resource \${res_file} ${CMAKE_BINARY_DIR}/src/nsxlib/Resource\${NSX_RESOURCE_NAME}.cpp)

    get_filename_component(NSX_RESOURCE_DIR \${res_file} DIRECTORY)
    get_filename_component(NSX_RESOURCE_TYPE \${NSX_RESOURCE_DIR} NAME)
    execute_process(COMMAND ${CMAKE_BINARY_DIR}/make_resources_map \${NSX_RESOURCE_TYPE} \${NSX_RESOURCE_NAME} ${CMAKE_BINARY_DIR}/src/nsxlib/ResourcesMap.cpp)
  endforeach ()"
)

add_custom_target(generate-nsx-resources COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/generate-nsx-resources.cmake)
add_dependencies(generate-nsx-resources make_resource)
add_dependencies(generate-nsx-resources make_resources_map)
  
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


