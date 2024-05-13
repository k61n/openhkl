set(lib openhkltables)

file(GLOB_RECURSE sources *.cpp)

add_library(${lib} ${sources})

target_link_options(${lib} PRIVATE "LINKER:-as-needed")

target_include_directories(
  openhkltables
  PUBLIC
  PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_link_libraries(openhkltables
  PUBLIC
  openhklbase
)
