if(WIN32)
    add_executable(nsxgui WIN32 main.cpp)
else()
    add_executable(nsxgui main.cpp)
endif()

target_include_directories(nsxgui
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty)

target_link_libraries(nsxgui PRIVATE QCR gui Qt5::Widgets)

if(WIN32)
    include(PackIFW)
endif()
