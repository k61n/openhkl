set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR}/include/nsxlib)
include_directories(${PYTHON_INCLUDE_DIRS})

if (NSX_DOCS)
    GeneratePythonDocs(pynsx_doc.i ${CMAKE_SOURCE_DIR}/swig)
else()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pynsx_doc.i "")
endif()

set_property(SOURCE pynsx.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE pynsx.i PROPERTY SWIG_MODULE_NAME pynsx)

swig_add_module(pynsx python pynsx.i)

swig_link_libraries(pynsx nsx)

file(GLOB swig_libraries ${CMAKE_CURRENT_BINARY_DIR}/*.so ${CMAKE_CURRENT_BINARY_DIR}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/*.py)
install(FILES ${swig_libraries} DESTINATION lib)



