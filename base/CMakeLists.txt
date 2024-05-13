set(lib openhklbase)

file(GLOB_RECURSE sources *.cpp *.c) # note: *.c needed for blosc

add_library(${lib} ${sources})

target_link_options(${lib} PRIVATE "LINKER:-as-needed")

target_include_directories(${lib}
  PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${QHULL_INCLUDE_DIR}
)

target_link_libraries(${lib}
  PRIVATE
  ${YAMLCPP_LIBRARIES}
  ${GSL_LIBRARIES}
  ${QHULL_LIBRARIES}
)
