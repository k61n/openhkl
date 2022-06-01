if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    # suppress warning from swig::container_owner<T>::back_reference(PyObject*, PyObject*)
    add_compile_options(-Wno-unused-parameter;-Wno-deprecated-declarations;-Wno-sometimes-uninitialized;-Wno-missing-field-initializers)
endif()

set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR} ${XSection_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIRS})

set_property(SOURCE pyohkl.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE pyohkl.i PROPERTY SWIG_MODULE_NAME pyohkl)

# Set the core headers path at the swig level
get_directory_property(PYOHKL_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
list(REMOVE_DUPLICATES PYOHKL_INCLUDE_DIRECTORIES)
set(PYOHKL_SWIG_INCLUDE_DIRECTORIES)
foreach(it ${PYOHKL_INCLUDE_DIRECTORIES})
  set(PYOHKL_SWIG_INCLUDE_DIRECTORIES ${PYOHKL_SWIG_INCLUDE_DIRECTORIES} "-I${it}")
endforeach()
set_property(SOURCE pyohkl.i PROPERTY SWIG_FLAGS ${PYOHKL_SWIG_INCLUDE_DIRECTORIES})

swig_add_library(pyohkl TYPE MODULE LANGUAGE python SOURCES pyohkl.i)

swig_link_libraries(pyohkl ${PYTHON_LIBRARIES} openhklcore ${XSection_LIBRARIES})

configure_file("${CMAKE_SOURCE_DIR}/swig/pypackage/__init__.py.in" "${CMAKE_BINARY_DIR}/swig/__init__.py" @ONLY)
