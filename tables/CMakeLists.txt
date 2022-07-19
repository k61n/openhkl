file(GLOB_RECURSE sources *.cpp)

add_library(openhkltables ${sources})

target_include_directories(
    openhkltables
    PUBLIC
    PRIVATE
    ${CMAKE_SOURCE_DIR})

target_link_libraries(openhkltables
    PUBLIC
    openhklbase
    )
