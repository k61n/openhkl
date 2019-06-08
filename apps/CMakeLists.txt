file(GLOB_RECURSE NSXQT_HEADERS ${CMAKE_SOURCE_DIR}/apps/*.h)
file(GLOB_RECURSE NSXQT_SOURCES ${CMAKE_SOURCE_DIR}/apps/*.cpp)
file(GLOB_RECURSE NSXQT_UIS     ${CMAKE_SOURCE_DIR}/apps/*.ui)
file(GLOB_RECURSE NSXQT_RCS *.qrc)

qt5_wrap_ui(NSXQT_UI_HEADERS ${NSXQT_UIS})

#library for use in the new gui
add_library(
    apps
    ${NSXQT_SOURCES}
    ${NSXQT_RCS}
    ${NSXQT_RESOURCES_CPP}
    ${NSXQT_BUNDLE_RESOURCES}
    ${NSXQT_HEADERS}
    ${NSXQT_UI_HEADERS})

target_include_directories(
    apps
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty/QCustomPlot
    ${CMAKE_BINARY_DIR}/core/generated
    )

target_link_libraries(
    apps
    ${QT_LIBRARIES}
    QCustomPlot
    ${OPENGL_LIBRARIES}
    ${NSX_LIBRARY})
