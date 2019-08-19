cmake_minimum_required(VERSION 3.6 FATAL_ERROR) # 3.6+ required by IFW for VS2015+
cmake_policy(VERSION 3.6)
if(POLICY CMP0078)
  cmake_policy(SET CMP0078 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
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
option(BUILD_TESTING "Build the tests" OFF)

###################################################################################################
#  Build type and compiler flags
###################################################################################################

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()

include(SetCompilerFlags)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(DEFAULT_BUILD_SHARED_LIBS ON)

if(COVERAGE)
    include(CheckCoverage)
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

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL PrintSupport REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

###################################################################################################
#  Other dependences
###################################################################################################

include(CheckDependencies)

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
        ${CMAKE_SOURCE_DIR}/devtools/check-line-length.py 100 ${allsources})
endif()

###################################################################################################
#  Build components
###################################################################################################

add_subdirectory(base)
add_subdirectory(tables)
add_subdirectory(core)

if(BUILD_NSX_GUI)
    find_package(QCR MODULE REQUIRED)
    message(STATUS "libQCR: FOUND=${QCR_FOUND}, VERSION=${QCR_VERSION}, LIB=${QCR_LIBRARIES}")
    add_subdirectory(3rdparty/QCustomPlot)
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

if(COVERAGE)
    list(APPEND ignore_dirs "'/usr/include/*'")
    list(APPEND ignore_dirs "'/usr/lib/*'")
    list(APPEND ignore_dirs "'${CMAKE_BINARY_DIR}/*'")
    list(APPEND ignore_dirs "'${CMAKE_SOURCE_DIR}/externals/*'")
    #list(APPEND ignore_dirs "'*/tests/*'")
    add_coverage_target(coverage "${ignore_dirs}")
endif()
