set(lib QCustomPlot)

add_compile_options(-Wno-error=deprecated-declarations)

add_library(${lib} QCustomPlot.cpp)

target_include_directories(${lib} PRIVATE ${CMAKE_CURRENT_SOURCE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS})

target_link_libraries(${lib} PRIVATE Qt5::Gui Qt5::Widgets Qt5::PrintSupport)
