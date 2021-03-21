cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
if(APPLE)
    list(APPEND CMAKE_MODULE_PATH /usr/local/opt/share/cmake)
endif()
include(PreventInSourceBuilds)

project(NSXTool VERSION 0.1.0)
message(STATUS "Building ${PROJECT_NAME} ${PROJECT_VERSION}")

set(CMAKE_PROJECT_DESCRIPTION # for display in installer
    "NSXTool - peak integration for neutron single crystal diffraction")

###################################################################################################
#  Options
###################################################################################################

option(BUILD_NSX_GUI "Build nsx based GUI application." ON)
option(BUILD_NSX_OLD_APPS "Build old GUI." OFF)
option(BUILD_OPTIMIZED_DEBUG "Set optimization level -O1 or -Og for debug build" OFF)
option(COVERAGE "Build code coverage report" OFF)
option(BUILD_WITH_OPENMP "Build with OpenMP support" OFF)
option(NSX_VALGRIND "Run tests with valgrind" OFF)
option(NSX_SANITIZE "Compile with clang's -fsanitize (must also set NSX_SANITIZER variable)" OFF)
option(NSX_PYTHON "Generate Python bindings" ON)
option(BUILD_TESTING "Build the tests" ON)
option(BUILD_DOCUMENTATION "Build Sphinx/Doxygen documentation" ON)
option(BUILD_PDF_DOCUMENTATION "Build Sphinx documentation using LaTeX" ON)

if(NOT DEFINED NSX_TESTDATA_DIR)
    message(FATAL_ERROR "Missing -DNSX_TESTDATA_DIR=<path>"
             " where <path> should point to directory that contains copy of"
             " https://computing.mlz-garching.de/download/testdata/nsx/")
elseif(NOT IS_DIRECTORY ${NSX_TESTDATA_DIR})
    message(FATAL_ERROR "Directory NSX_TESTDATA_DIR=${NSX_TESTDATA_DIR} does not exist")
else()
    message(STATUS "Test data expected in NSX_TESTDATA_DIR=${NSX_TESTDATA_DIR}")
endif()
set(NSX_TESTDATA_DIR /testdata CACHE PATH "Directory where to cache test data")

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

if(APPLE)
    add_compile_options(-mmacosx-version-min=10.14)
endif()
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
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-sign-compare)
    add_compile_options(-Wno-overlength-strings) # WAITING for xsection to remove overlong strings
    # exception for swig-generated code:
    #add_compile_options(-Wno-error=sometimes-uninitialized)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-incomplete>)
    add_compile_options(-pthread)
    add_definitions(-DEIGEN_FFTW_DEFAULT)
    add_definitions(-D_USE_MATH_DEFINES)
elseif(DEFINED MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4348 /wd4127 /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4348 /wd4127 /MP")
    add_compile_options(/W2) # reasonable warning level
    add_definitions(/DH5_BUILT_AS_DYNAMIC_LIB)
    add_definitions(/DBOOST_ALL_NO_LIB)
    add_definitions(/D_USE_MATH_DEFINES)
    add_definitions(/DNSXTOOL_EXPORT)
else()
    message(WARNING "Unsupported compiler, lacking compiler-specific parameterization")
endif()

# code sanitizer
if(NSX_SANITIZE)
    message("The sanitize options are: thread, memory, undefined, dataflow, cfi, safe-stack")
    message(STATUS "The sanitize option '${NSX_SANITIZER}' has been selected")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${NSX_SANITIZER} -fsanitize-recover=all")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(DEFAULT_BUILD_SHARED_LIBS ON)

if(COVERAGE)
    include(CoverageFunction)
    include(NSXCoverage)
endif()

###################################################################################################
#  Qt
###################################################################################################

set(OpenGL_GL_PREFERENCE LEGACY)
set(QT_USE_QTOPENGL TRUE)
find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL:")
message(STATUS "  libraries: ${OPENGL_LIBRARIES}")
message(STATUS "  headers: ${OPENGL_INCLUDE_DIR}")

if (APPLE)
    find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL PrintSupport REQUIRED
        HINTS /opt/local/ /opt/local/lib /opt/local/include)
else()
    find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL PrintSupport REQUIRED)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

###################################################################################################
#  Other dependences
###################################################################################################

# QCustomPlot: As long as there is no homebrew package for MacOS, we provide it as 3rdparty code:
add_subdirectory(3rdparty/qcustomplot)
set(QCustomPlot_LIBRARIES qcustomplot)
set(QCustomPlot_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/qcustomplot)
#find_package(QCustomPlot MODULE REQUIRED)
#message(STATUS "libQCustomPlot: FOUND=${QCustomPlot_FOUND},
#        VERSION=${QCustomPlot_VERSION}, LIB=${QCustomPlot_LIBRARIES},
#        INCLUDE_DIR=${QCustomPlot_INCLUDE_DIR}")

find_package(Eigen3 MODULE REQUIRED)
message(STATUS "Found Eigen3: QHULL_VERSION=${EIGEN3_VERSION}, QHULL_INCLUDE_DIR=${EIGEN3_INCLUDE_DIR}")

find_package(Qhull MODULE REQUIRED)
message(STATUS "Found QHull: QHULL_INCLUDE_DIR=${QHULL_INCLUDE_DIR}")

include(CheckDependencies)
add_subdirectory(3rdparty/xsection)
#set(XSection_LIBRARIES XSection)
#set(XSection_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/xsection)

###################################################################################################
#  Prepare for tests; static code analysis
###################################################################################################

include(CTest)

if(NSX_PYTHON AND NOT MSVC AND BUILD_TESTING)
    # ensure that no code line has more than 100 chars
    set(allsources "")
    foreach(dir base tables core)
        file(GLOB_RECURSE sources ${dir}/*.cpp ${dir}/*.h)
        list(APPEND allsources ${sources})
    endforeach()
    add_test(NAME "CodeLineLength"
        COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/devtools/check-line-length.py 200 ${allsources})
endif()

###################################################################################################
#  Build documentation
###################################################################################################

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    find_package(Sphinx)
    message(STATUS "Found Sphinx: SPHINX_EXECUTABLE=${SPHINX_EXECUTABLE}")
    find_package(LATEX COMPONENTS PDFLATEX BIBTEX)
    message(STATUS "PDFLATEX_COMPILER=${PDFLATEX_COMPILER} BIBTEX_COMPILER=${BIBTEX_COMPILER}")
endif()

###################################################################################################
#  Build components
###################################################################################################
add_subdirectory(base)
add_subdirectory(tables)
add_subdirectory(core)

if(BUILD_NSX_GUI)
    add_subdirectory(gui)
    add_subdirectory(main)
    if(BUILD_NSX_OLD_APPS)
        add_subdirectory(apps)
    endif()
endif()

if(BUILD_TESTING) # option defined by CTest and enabled by default
    add_subdirectory(test/data)
    add_subdirectory(test/cpp)
endif()

if(NSX_PYTHON)
    add_subdirectory(swig)
    if(BUILD_TESTING)
        add_subdirectory(test/python)
    endif()
endif()

if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

###################################################################################################
#  Pack
###################################################################################################

if (APPLE)
  install(TARGETS NSXTool DESTINATION .)
elseif(WIN32)
  # TODO
else()
  set_property(TARGET NSXTool PROPERTY INSTALL_RPATH_USE_LINK_PATH TRUE)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  install(TARGETS NSXTool DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

include(Pack)
