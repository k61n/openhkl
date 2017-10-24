set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR}/include/nsxlib)
include_directories(${PYTHON_INCLUDE_DIRS})

if (NSX_DOCS)
    GeneratePythonDocs(pynsx_doc.i ${CMAKE_SOURCE_DIR}/swig)
    set (include_doxy_swig_doc "%include \"pynsx_doc.i\"")
else()
    set (include_doxy_swig_doc "")
endif()

configure_file(pynsx.i.in ${CMAKE_CURRENT_BINARY_DIR}/pynsx.i)

set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/pynsx.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/pynsx.i PROPERTY SWIG_MODULE_NAME pynsx)

swig_add_module(pynsx python ${CMAKE_CURRENT_BINARY_DIR}/pynsx.i)

swig_link_libraries(pynsx nsx)

file(GLOB swig_libraries ${CMAKE_CURRENT_BINARY_DIR}/*.so ${CMAKE_CURRENT_BINARY_DIR}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/*.py)
install(FILES ${swig_libraries} DESTINATION lib)



