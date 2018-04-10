if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

GET_SUBDIRECTORIES(NSXQT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# add project root directory to include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${NSXQT_SUBDIRS}
                    ${CMAKE_BINARY_DIR}/include)


# Disable console on Windows
if(WIN32)
  set(GUI_TYPE WIN32)
endif(WIN32)

# Find a few Qt submodules
find_package(Qt5Core REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

#Find OpenGL
find_package(OpenGL REQUIRED)

# Find recursively all the header files of NSXQt
file(GLOB_RECURSE NSXQT_HEADERS *.h)

# Find recursively all the cpp source files of NSXQt
file(GLOB_RECURSE NSXQT_SOURCES *.cpp)

if (${CMAKE_VERSION} VERSION_LESS 2.8.11)
  # Add the include directories for the Qt 5 Widgets module to the compile lines.
  # There might be some redundancies.
  include_directories(${Qt5Core_INCLUDE_DIRS})
  include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
  include_directories(${Qt5OpenGL_INCLUDE_DIRS})
  include_directories(${Qt5Widgets_INCLUDE_DIRS})
  include_directories(${Qt5Gui_INCLUDE_DIRS})

  # Use the compile definitions defined in the Qt 5 Widgets module
  # There might be some redundancies.
  add_definitions(${Qt5Core_DEFINITIONS})
  add_definitions(${Qt5PrintSupport_DEFINITIONS})
  add_definitions(${Qt5OpenGL_DEFINITIONS})
  add_definitions(${Qt5Widgets_DEFINITIONS})
  add_definitions(${Qt5Gui_DEFINITIONS})

  # Add compiler flags for building executables (i.e. -fPIE)
  # There might be some redundancies.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5PrintSupport_EXECUTABLE_COMPILE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")

  # Generate the Qt moc files
  # All the sources files that are not Qt Q_OBJECT will trigger a warning
  # and won't be compiled as moc objects.
  qt5_wrap_cpp(mocs ${NSXQT_HEADERS})
  list(APPEND NSXQT_SOURCES ${mocs})
else()
  # Find includes in corresponding build directories
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  # Handle automatically moc files
  set(CMAKE_AUTOMOC ON)
  # Handle automatically ui files
  set(CMAKE_AUTOUIC OFF)
  # Specify that the app will use OpenGL
  set(QT_USE_QTOPENGL TRUE)
  # Handle automatically resources files
  set(CMAKE_AUTORCC ON)
endif()

# Find recursively all the Qt ui files of NSXQt
file(GLOB_RECURSE NSXQT_UIS *.ui)

# Build the headers files for all Qt ui files
qt5_wrap_ui(ui_headers ${NSXQT_UIS})

# Find recursively all the Qt resources files of NSXQt
file(GLOB_RECURSE NSXQT_RCS *.qrc)
# "Compile" the resources files
qt5_add_resources(resources ${NSXQT_RCS})

# Build the nsxqt executable
add_executable(nsxqt ${GUI_TYPE} ${NSXQT_SOURCES} ${NSXQT_HEADERS} ${ui_headers} ${resources})

# Link the nsxqt executable with the Qt5 libraries in use.
target_link_libraries(
  nsxqt
  Qt5::Core
  Qt5::PrintSupport
  Qt5::Gui
  Qt5::Widgets
  Qt5::OpenGL
  ${OPENGL_LIBRARIES}
  nsx
  ${HDF5_LIBRARIES}
)

# Install section, installing libraries
install(TARGETS nsxqt DESTINATION bin)

set(qmake_sources "")
set(qmake_headers "")
set(qmake_uis "")
set(qmake_include_dirs "")

foreach(source_file ${NSXQT_SOURCES})
  set(qmake_sources "${qmake_sources} ${source_file}")
endforeach(source_file)

foreach(source_file ${NSXQT_HEADERS})
  set(qmake_headers "${qmake_headers} ${source_file}")
endforeach(source_file)

foreach(source_file ${NSXQT_UIS})
  set(qmake_uis "${qmake_uis} ${source_file}")
endforeach(source_file)

get_property(dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  if (NOT ${dir} STREQUAL "/usr/include")
    set(qmake_include_dirs "${qmake_include_dirs} ${dir}")
  endif()
endforeach(dir)

# generate the qmake project file automatically
configure_file(NSXQt.pro.in ${CMAKE_CURRENT_BINARY_DIR}/NSXQt.pro)

# tidy
if(NSX_TIDY)
  add_tidy_target(nsxqt)
endif()
