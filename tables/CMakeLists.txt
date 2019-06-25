file(GLOB_RECURSE sources *.cpp)

add_library(nsxtables ${sources})

target_include_directories(
    nsxtables
    PRIVATE
    ${CMAKE_SOURCE_DIR})

target_link_libraries(nsxtables
    PUBLIC
    nsxbase
    ${Qt5Core_LIBRARIES}
    )
