if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
${CMAKE_SOURCE_DIR}/nsxlib/chemistry/
${CMAKE_SOURCE_DIR}/nsxlib/crystal/
${CMAKE_SOURCE_DIR}/nsxlib/data/
${CMAKE_SOURCE_DIR}/nsxlib/geometry/
${CMAKE_SOURCE_DIR}/nsxlib/imaging/
${CMAKE_SOURCE_DIR}/nsxlib/instrument/
${CMAKE_SOURCE_DIR}/nsxlib/kernel/
${CMAKE_SOURCE_DIR}/nsxlib/utils/
)

# Disable console on Windows
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)

# Find a few Qt submodules
find_package(Qt5Core REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

# Find recursively all the header files of NSXQt
file(GLOB_RECURSE lattice_refiner_headers ${CMAKE_SOURCE_DIR}/apps/LatticeRefiner/*.h)
# Find recursively all the cpp source files of NSXQt
file(GLOB_RECURSE lattice_refiner_sources ${CMAKE_SOURCE_DIR}/apps/LatticeRefiner/*.cpp)

if (${CMAKE_VERSION} VERSION_LESS 2.8.11)

    # Add the include directories for the Qt 5 Widgets module to the compile lines.
    # There might be some redundancies.
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    include_directories(${Qt5Gui_INCLUDE_DIRS})

    # Use the compile definitions defined in the Qt 5 Widgets module
    # There might be some redundancies.
    add_definitions(${Qt5Core_DEFINITIONS})
    add_definitions(${Qt5PrintSupport_DEFINITIONS})
    add_definitions(${Qt5Widgets_DEFINITIONS})
    add_definitions(${Qt5Gui_DEFINITIONS})

    # Add compiler flags for building executables (i.e. -fPIE)
    # There might be some redundancies.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5PrintSupport_EXECUTABLE_COMPILE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")

    # Generate the Qt moc files
    # All the sources files that are not Qt Q_OBJECT will trigger a warning and won't be compiled as moc objects.
    qt5_wrap_cpp(mocs ${lattice_refiner_headers})

    # Append the moc cpp files to the NSXQt sources
    list(APPEND lattice_refiner_sources ${mocs})

else ()

    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    # Handle automatically moc files
    set(CMAKE_AUTOMOC ON)
    # Handle automatically ui files
    set(CMAKE_AUTOUIC ON)
    # Handle automatically resources files
    set(CMAKE_AUTORCC ON)

endif()

# Find recursively all the Qt ui files of Lattice Refiner
file(GLOB_RECURSE lattice_refiner_uis ${CMAKE_SOURCE_DIR}/apps/LatticeRefiner/*.ui)
# Build the headers files for all Qt ui files
qt5_wrap_ui(ui_headers ${lattice_refiner_uis})

# Find recursively all the Qt resources files of Lattice Refiner
file(GLOB_RECURSE lattice_refiner_rcs ${CMAKE_SOURCE_DIR}/apps/LatticeRefiner/*.qrc)
# "Compile" the resources files
qt5_add_resources(resources ${lattice_refiner_rcs})

# For info use 'objdump -x lattice_refiner | grep RPATH' to the installed executable (not the built one)
# to check that the RPATH is correctly set

# Use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# When building, don't use the install RPATH already (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# Set the install RPATH
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

# Build the lattice_refiner executable
add_executable(lattice_refiner ${GUI_TYPE} ${lattice_refiner_sources} ${lattice_refiner_headers} ${ui_headers} ${resources})

# Link the lattice_refiner executable with the Qt5 libraries in use.
target_link_libraries(lattice_refiner Qt5::Core Qt5::PrintSupport Qt5::Gui Qt5::Widgets nsx)

# Install section, installing libraries
install(TARGETS lattice_refiner DESTINATION bin)

