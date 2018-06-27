# Find OpenGL
find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL in ${OPENGL_LIBRARIES}")

# Find the Qt framework on which nsxqt is depending on
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(NOT ${Qt5Core_FOUND} OR NOT ${Qt5Gui_FOUND} OR NOT ${Qt5OpenGL_FOUND} OR NOT ${Qt5PrintSupport_FOUND} OR NOT ${Qt5Widgets_FOUND})
    message(FATAL_ERROR "Could not find one or several Qt frameworks")
endif()

# Specify that the nsxqt will use OpenGL
set(QT_USE_QTOPENGL TRUE)

get_target_property(Qt5Core_location Qt5::Core LOCATION_Release)
message(STATUS "  ${Qt5Core_LIBRARIES}(${Qt5Core_VERSION}) ${Qt5Core_location}")
get_target_property(Qt5Gui_location Qt5::Gui LOCATION_Release)
message(STATUS "  ${Qt5Gui_LIBRARIES}(${Qt5Gui_VERSION}) ${Qt5Gui_location}")
get_target_property(Qt5OpenGL_location Qt5::OpenGL LOCATION_Release)
message(STATUS "  ${Qt5OpenGL_LIBRARIES}(${Qt5OpenGL_VERSION}) ${Qt5OpenGL_location}")
get_target_property(Qt5PrintSupport_location Qt5::PrintSupport LOCATION_Release)
message(STATUS "  ${Qt5PrintSupport_LIBRARIES}(${Qt5PrintSupport_VERSION}) ${Qt5PrintSupport_location}")
get_target_property(Qt5Widgets_location Qt5::Widgets LOCATION_Release)
message(STATUS "  ${Qt5Widgets_LIBRARIES}(${Qt5Widgets_VERSION}) ${Qt5Widgets_location}")

message(STATUS "  Includes: ${Qt5Widgets_INCLUDE_DIRS}")

set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5OpenGL_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} ${Qt5Widgets_LIBRARIES})

GET_SUBDIRECTORIES(NSXQT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Find recursively all the header files of NSXQt
file(GLOB_RECURSE NSXQT_HEADERS ${CMAKE_SOURCE_DIR}/apps/nsxqt/*.h)

# Find recursively all the cpp source files of NSXQt
file(GLOB_RECURSE NSXQT_SOURCES ${CMAKE_SOURCE_DIR}/apps/nsxqt/*.cpp)

# Find recursively all the Qt ui files of NSXQt
file(GLOB_RECURSE NSXQT_UIS ${CMAKE_SOURCE_DIR}/apps/nsxqt/*.ui)

# Build the headers files for all Qt ui files
qt5_wrap_ui(NSXQT_UI_HEADERS ${NSXQT_UIS})

# Find recursively all the Qt resources files of NSXQt
file(GLOB_RECURSE NSXQT_RCS *.qrc)

get_directory_property(PYNSX_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
list(REMOVE_DUPLICATES PYNSX_INCLUDE_DIRECTORIES)

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
  qt5_wrap_cpp(NSXQT_MOC_CPP ${NSXQT_UI_HEADERS})

  # "Compile" the resources files
  qt5_add_resources(NSXQT_RESOURCES_CPP ${NSXQT_RCS})
  
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

