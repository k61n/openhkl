get_subdirectories(NSXQT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Find recursively all the header files of NSXQt
file(GLOB_RECURSE NSXQT_HEADERS ${CMAKE_SOURCE_DIR}/apps/*.h)

# Find recursively all the cpp source files of NSXQt
file(GLOB_RECURSE NSXQT_SOURCES ${CMAKE_SOURCE_DIR}/apps/*.cpp)

# Find recursively all the Qt ui files of NSXQt
file(GLOB_RECURSE NSXQT_UIS ${CMAKE_SOURCE_DIR}/apps/*.ui)

# Build the headers files for all Qt ui files
qt5_wrap_ui(NSXQT_UI_HEADERS ${NSXQT_UIS})

# Find recursively all the Qt resources files of NSXQt
file(GLOB_RECURSE NSXQT_RCS *.qrc)

get_directory_property(PYNSX_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
list(REMOVE_DUPLICATES PYNSX_INCLUDE_DIRECTORIES)

get_subdirectories(NSXQT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

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
