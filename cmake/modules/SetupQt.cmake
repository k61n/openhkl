# Find OpenGL
find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL:")
message(STATUS "  libraries: ${OPENGL_LIBRARIES}")
message(STATUS "  headers: ${OPENGL_INCLUDE_DIR}")

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

message(STATUS "Found Qt:")
message(STATUS "  version: ${Qt5Core_VERSION}")
message(STATUS "  libraries:")

get_target_property(Qt5Core_location Qt5::Core LOCATION_Release)
message(STATUS "    ${Qt5Core_LIBRARIES}: ${Qt5Core_location}")

get_target_property(Qt5Gui_location Qt5::Gui LOCATION_Release)
message(STATUS "    ${Qt5Gui_LIBRARIES}: ${Qt5Gui_location}")

get_target_property(Qt5OpenGL_location Qt5::OpenGL LOCATION_Release)
message(STATUS "    ${Qt5OpenGL_LIBRARIES}: ${Qt5OpenGL_location}")

get_target_property(Qt5PrintSupport_location Qt5::PrintSupport LOCATION_Release)
message(STATUS "    ${Qt5PrintSupport_LIBRARIES}: ${Qt5PrintSupport_location}")

get_target_property(Qt5Widgets_location Qt5::Widgets LOCATION_Release)
message(STATUS "    ${Qt5Widgets_LIBRARIES}: ${Qt5Widgets_location}")

message(STATUS "  headers: ${Qt5Widgets_INCLUDE_DIRS}")

set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5OpenGL_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} ${Qt5Widgets_LIBRARIES})

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
