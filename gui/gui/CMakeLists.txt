set(lib gui)

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    )

target_link_libraries(${lib}
    PRIVATE
    Qt5::Network
    Qt5::Widgets
    qcr
    )
