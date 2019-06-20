set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL:")
message(STATUS "  libraries: ${OPENGL_LIBRARIES}")
message(STATUS "  headers: ${OPENGL_INCLUDE_DIR}")

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL PrintSupport)

set(QT_USE_QTOPENGL TRUE)

# TODO: remove when apps/ is gone
set(QT_LIBRARIES
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    # ${Qt5Network_LIBRARIES}
    )

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
