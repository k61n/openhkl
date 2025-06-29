cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
if(APPLE)
    list(APPEND CMAKE_MODULE_PATH /usr/local/opt/share/cmake)
endif()
include(PreventInSourceBuilds)
include(version)
message(STATUS "Git branch ${GIT_BRANCH}")
message(STATUS "Git commit hash ${COMMIT_HASH}")

project(OpenHKL VERSION 0.1.0)
message(STATUS "Building ${PROJECT_NAME} ${PROJECT_VERSION}")

set(CMAKE_PROJECT_DESCRIPTION # for display in installer
    "OpenHKL - peak integration for single crystal diffraction")

###################################################################################################
#  Options
###################################################################################################

option(BUILD_OHKL_GUI "Build OpenHKL GUI application." ON)
option(BUILD_WITH_QT6 "Build using QT6" OFF)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)
option(COVERAGE "Build code coverage report" OFF)
option(OHKL_VALGRIND "Run tests with valgrind" OFF)
option(OHKL_SANITIZE "Compile with clang's -fsanitize (must also set OHKL_SANITIZER variable)" OFF)
option(OHKL_PYTHON "Generate Python bindings" ON)
option(BUILD_TESTING "Build the tests" ON)
option(BUILD_DOCUMENTATION "Build Sphinx/Doxygen documentation" ON)
option(BUILD_PDF_DOCUMENTATION "Build Sphinx documentation using LaTeX" ON)
option(OHKL_TIDY "Upon 'make', clang-tidy will print warnings as specified in .clang-tidy" OFF)
option(OHKL_TEST_INTEGRATORS "Build intensive integrator tests" OFF)
option(OHKL_FULL_WORKFLOW_TEST "Add full workflow test to ctest" OFF)

# Path to test data directory must provided via -DOHKL_TESTDATA_DIR or from cache.
if (BUILD_TESTING)
    if(NOT DEFINED OHKL_TESTDATA_DIR)
        set(OHKL_TESTDATA_DIR "test/data")
    elseif(NOT IS_DIRECTORY ${OHKL_TESTDATA_DIR})
        message(FATAL_ERROR "Directory OHKL_TESTDATA_DIR=${OHKL_TESTDATA_DIR} does not exist")
    else()
        message(STATUS "Test data expected in OHKL_TESTDATA_DIR=${OHKL_TESTDATA_DIR}")
    endif()
endif()
# Cache the variable. The default value, required by set(..CACHE..) syntax,  will never be used.
# set(OHKL_TESTDATA_DIR /testdata CACHE PATH "Directory where to cache test data")

# Check compatibility of options
if(OHKL_TIDY AND OHKL_PYTHON)
    message(FATAL_ERROR "OHKL_TIDY is incompatible with OHKL_PYTHON")
endif()

###################################################################################################
#  Build type and compiler flags
###################################################################################################

if(UNIX AND NOT APPLE)
    set(ARCHITECTURE linux)
    set(LINUX TRUE)
elseif(APPLE)
    set(ARCHITECTURE macos)
elseif(WIN32)
    set(ARCHITECTURE windows)
else()
    message(FATAL "Unsupported architecture")
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()

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

        if(CMAKE_BUILD_TYPE MATCHES Debug)
            add_compile_options(-ggdb -rdynamic)
        endif()

        if(BUILD_OPTIMIZED_DEBUG)
            add_compile_options(-Og)
        endif()
    endif()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-sign-compare -Wno-error=return-type)
    add_compile_options(-Wno-overlength-strings) # WAITING for xsection to remove overlong strings
    # exception for swig-generated code:
    #add_compile_options(-Wno-error=sometimes-uninitialized)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-incomplete>)
    # add_compile_options(-pthread)
    add_definitions(-DEIGEN_FFTW_DEFAULT)
    add_definitions(-D_USE_MATH_DEFINES)
elseif(DEFINED MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4348 /wd4127 /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4348 /wd4127 /MP")
    add_compile_options(/W2) # reasonable warning level
    add_definitions(/DH5_BUILT_AS_DYNAMIC_LIB)
    add_definitions(/D_USE_MATH_DEFINES)
    add_definitions(/DOHKLTOOL_EXPORT)
else()
    message(WARNING "Unsupported compiler, lacking compiler-specific parameterization")
endif()
if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    add_definitions(-DDATAROOTDIR="${CMAKE_INSTALL_FULL_DATAROOTDIR}")
endif()

# code sanitizer
if(OHKL_SANITIZE)
    message("The sanitize options are: thread, memory, undefined, dataflow, cfi, safe-stack")
    message(STATUS "The sanitize option '${OHKL_SANITIZER}' has been selected")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${OHKL_SANITIZER} -fsanitize-recover=all")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${OHKL_SANITIZER} -fsanitize-recover=all")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(DEFAULT_BUILD_SHARED_LIBS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

if(COVERAGE)
    include(CoverageFunction)
    include(OHKLCoverage)
endif()

###################################################################################################
#  Dependencies
###################################################################################################

include(CheckDependencies)

###################################################################################################
#  3rd party dependencies
###################################################################################################

# QCustomPlot: there is currently no homebrew package for QCustomPlot
add_subdirectory(3rdparty/qcustomplot)
set(QCustomPlot_LIBRARIES qcustomplot)
set(QCustomPlot_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/qcustomplot)
#find_package(QCustomPlot MODULE REQUIRED)
#message(STATUS "libQCustomPlot: FOUND=${QCustomPlot_FOUND},
#        VERSION=${QCustomPlot_VERSION}, LIB=${QCustomPlot_LIBRARIES},
#        INCLUDE_DIR=${QCustomPlot_INCLUDE_DIR}")

add_subdirectory(3rdparty/ccp4)
set(Mtz_LIBRARIES mtz)
set(Mtz_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/ccp4)


###################################################################################################
#  Prepare for tests; static code analysis
###################################################################################################

include(CTest)

if(OHKL_PYTHON AND NOT MSVC AND BUILD_TESTING)
    # ensure that no code line has more than 100 chars
    set(allsources "")
    foreach(dir base tables core)
        file(GLOB_RECURSE sources ${dir}/*.cpp ${dir}/*.h)
        list(APPEND allsources ${sources})
    endforeach()
    add_test(NAME "CodeLineLength"
        COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/devtools/check-line-length.py 200 ${allsources})
endif()

# Ignore tests specified in CTestCustom.cmake
if (NOT OHKL_TEST_INTEGRATORS)
  configure_file(${CMAKE_SOURCE_DIR}/cmake/CTestCustom.cmake ${CMAKE_BINARY_DIR})
endif()

###################################################################################################
#  Build documentation
###################################################################################################

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    find_package(Sphinx)
    find_package(LATEX COMPONENTS PDFLATEX BIBTEX)
    message(STATUS "PDFLATEX_COMPILER=${PDFLATEX_COMPILER} BIBTEX_COMPILER=${BIBTEX_COMPILER}")
endif()

###################################################################################################
#  Build components
###################################################################################################

# from here on our own code, occasionally scrutinized by clang-tidy
if(OHKL_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy") # has effect only if compiler is clang; uses .clang-tidy
endif()

configure_file("manifest.h.in" "${CMAKE_BINARY_DIR}/manifest.h")

add_subdirectory(base)
add_subdirectory(tables)
add_subdirectory(core)

if(BUILD_OHKL_GUI)
    add_subdirectory(gui)
    add_subdirectory(main)
endif()

if(BUILD_TESTING) # option defined by CTest and enabled by default
    add_subdirectory(test/data)
    add_subdirectory(test/cpp)
endif()

if(OHKL_PYTHON)
    set(SWIG_INSTALL_PATH "${CMAKE_BINARY_DIR}/swig")
    add_subdirectory(swig)
    add_subdirectory(scripts)
    if(BUILD_TESTING)
        add_subdirectory(test/python)
        if (OHKL_FULL_WORKFLOW_TEST)
            add_subdirectory(test/python/nightly)
        endif()
    endif()
endif()

if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

###################################################################################################
#  Pack
###################################################################################################

if (APPLE)
  install(TARGETS OpenHKL DESTINATION .)
elseif(WIN32)
  install(TARGETS OpenHKL DESTINATION .)
else()
  set_property(TARGET OpenHKL PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
  install(TARGETS OpenHKL DESTINATION ${CMAKE_INSTALL_BINDIR})
  configure_file(OpenHKL.desktop.in ${CMAKE_BINARY_DIR}/OpenHKL.desktop)
  install(FILES ${CMAKE_BINARY_DIR}/OpenHKL.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
  install(FILES ${CMAKE_SOURCE_DIR}/main/images/icons/OpenHKL.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/instruments DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/openhkl)
endif()

if (NOT APPLE)
  include(Pack)
endif ()
