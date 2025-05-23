file(GLOB_RECURSE sources *.cpp *.c) # note: *.c needed for blosc

add_library(openhklbase ${sources})

target_include_directories(openhklbase PRIVATE ${CMAKE_SOURCE_DIR} ${QHULL_INCLUDE_DIR})

target_link_libraries(openhklbase
    PRIVATE
    ${YAMLCPP_LIBRARIES}
    ${GSL_LIBRARIES}
    ${QHULL_LIBRARIES}
    ${NLOPT_LIBRARIES}
    )
