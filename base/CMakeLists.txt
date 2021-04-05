file(GLOB_RECURSE sources *.cpp *.c) # note: *.c needed for blosc

add_library(nsxbase ${sources})

target_include_directories(nsxbase PRIVATE ${CMAKE_SOURCE_DIR} ${QHULL_INCLUDE_DIR})

target_link_libraries(nsxbase
    PRIVATE
    ${YAMLCPP_LIBRARIES}
    ${GSL_LIBRARIES}
    ${QHULL_LIBRARIES}
    )
