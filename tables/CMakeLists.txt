file(GLOB_RECURSE sources *.cpp)

add_library(nsxtables ${sources})

target_include_directories(
    nsxtables
    PRIVATE
    ${XSection_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR})

target_link_libraries(nsxtables
    PUBLIC
    nsxbase
    ${XSection_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    )
