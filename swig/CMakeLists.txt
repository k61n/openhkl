set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})

set_property(SOURCE pynsx.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE pynsx.i PROPERTY SWIG_MODULE_NAME pynsx)
swig_add_module(pynsx python pynsx.i)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/nsxlib)

swig_link_libraries(swig_test nsxlib)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/run_test.py ${CMAKE_CURRENT_BINARY_DIR}/run_test.py COPYONLY)


add_test(NAME swig_test COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/run_test.py)

