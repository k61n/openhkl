set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/nsxlib)
include_directories(${PYTHON_INCLUDE_DIRS})

set_property(SOURCE pynsx.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE pynsx.i PROPERTY SWIG_MODULE_NAME pynsx)
swig_add_module(pynsx python pynsx.i)



swig_link_libraries(pynsx nsx)

file(GLOB swig_libraries ${CMAKE_CURRENT_BINARY_DIR}/*.so ${CMAKE_CURRENT_BINARY_DIR}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/*.py)
install(FILES ${swig_libraries} DESTINATION lib)




