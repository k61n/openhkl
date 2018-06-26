set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR}/nsxlib/include/nsxlib)
include_directories(${PYTHON_INCLUDE_DIRS})

if (NSX_DOCS)
    GeneratePythonDocs(pynsx_doc.i ${CMAKE_SOURCE_DIR}/swig)
else()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pynsx_doc.i "")
endif()

set_property(SOURCE pynsx.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE pynsx.i PROPERTY SWIG_MODULE_NAME pynsx)

if(${CMAKE_VERSION} VERSION_GREATER 3.8)
    swig_add_library(pynsx TYPE MODULE LANGUAGE python SOURCES pynsx.i)
else()
    swig_add_module(pynsx python pynsx.i)
endif()

swig_link_libraries(pynsx nsx)

install(TARGETS ${SWIG_MODULE_pynsx_REAL_NAME} DESTINATION ${PYTHON_SITE})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pynsx.py DESTINATION ${PYTHON_SITE})
