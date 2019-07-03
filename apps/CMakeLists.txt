# TODO: remove when apps/ is gone
set(QT_LIBRARIES
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5OpenGL_LIBRARIES}
    ${Qt5PrintSupport_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    # ${Qt5Network_LIBRARIES}
    )

file(GLOB_RECURSE NSXQT_SOURCES ${CMAKE_SOURCE_DIR}/apps/*.cpp)
file(GLOB_RECURSE NSXQT_UIS     ${CMAKE_SOURCE_DIR}/apps/*.ui)
file(GLOB_RECURSE NSXQT_RCS *.qrc)

qt5_wrap_ui(NSXQT_UI_HEADERS ${NSXQT_UIS})

#library for use in the new gui
add_library(
    apps
    ${NSXQT_SOURCES}
    ${NSXQT_RCS}
    ${NSXQT_UI_HEADERS})

target_include_directories(
    apps
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty/QCustomPlot
    )

target_link_libraries(
    apps
    nsxcore
    ${QT_LIBRARIES}
    QCustomPlot
    ${OPENGL_LIBRARIES}
    )
