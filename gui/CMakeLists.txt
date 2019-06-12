set(lib gui)

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${Qt5Widgets_INCLUDE_DIRS}
    )

target_link_libraries(${lib}
    PRIVATE
    nsxcore
    QCR
    QCustomPlot
    )
