set(lib nsxgui)

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${QCustomPlot_INCLUDE_DIR}
    )

target_link_libraries(${lib}
    PRIVATE
    nsxcore
    ${QCR_LIBRARIES}
    ${QCustomPlot_LIBRARIES}
    )
