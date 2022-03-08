set(lib nsxgui)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-deprecated-copy) # to suppress warnings from Qt5 (at least <= 5.12.5)
endif()

file(GLOB_RECURSE sources *.cpp)
add_library(${lib} ${sources})

target_include_directories(${lib}
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    ${QT_INCLUDES}
    ${QCustomPlot_INCLUDE_DIR}
    )

target_link_libraries(${lib}
    PRIVATE
    nsxcore
    ${QCustomPlot_LIBRARIES}
    ${QT_LIBS}
    )
