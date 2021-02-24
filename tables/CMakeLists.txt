file(GLOB_RECURSE sources *.cpp)

add_library(nsxtables ${sources})

target_include_directories(
    nsxtables
    PUBLIC
    ${XSection_INCLUDE_DIR}
    PRIVATE
    ${CMAKE_SOURCE_DIR})

target_link_libraries(nsxtables
    PUBLIC
    nsxbase
    ${XSection_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    )
