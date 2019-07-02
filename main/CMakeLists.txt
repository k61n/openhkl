set(app NSXTool)

if(WIN32)
    add_executable(${app} WIN32 main.cpp)
else()
    add_executable(${app} main.cpp)
endif()

target_include_directories(${app}
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty
    )

target_link_libraries(${app} PRIVATE ${QCR_LIBRARIES} nsxgui Qt5::Widgets)

if(WIN32)
    include(PackIFW)
endif()
