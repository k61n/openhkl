set(app NSXTool)

if(WIN32)
    add_executable(${app} WIN32 main.cpp resources.qrc)
elseif(APPLE)
    add_executable(${app} MACOSX_BUNDLE main.cpp resources.qrc)
else()
    add_executable(${app} main.cpp resources.qrc)
endif()

target_include_directories(${app}
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    ${XSection_INCLUDE_DIR}
    ${Qt6Widgets_INCLUDE_DIRS}
    )

target_link_libraries(${app} PRIVATE ${Qt6Widgets_LIBRARIES} nsxgui)

if(WIN32)
    include(PackIFW)
endif()
