file(GLOB_RECURSE sources *.cpp)
add_library(nsxbase ${sources})

target_include_directories(nsxbase PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(nsxbase
    PRIVATE
    ${YAML_LIBRARIES}
    ${GSL_LIBRARIES}
    )
