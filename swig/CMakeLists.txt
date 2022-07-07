if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    # suppress warning from swig::container_owner<T>::back_reference(PyObject*, PyObject*)
    add_compile_options(-Wno-unused-parameter;-Wno-deprecated-declarations;-Wno-sometimes-uninitialized;-Wno-missing-field-initializers)
endif()

set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "SWIG: output dir = '${CMAKE_SWIG_OUTDIR}'")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})

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

swig_link_libraries(pyohkl openhklcore ${Python3_LIBRARIES})

if(WIN32)
    # On Windows, the location of the produced library depends
    # on the build configuration; hence, for the correct functioning of
    # the Python package, the library and its Python wrapper must be
    # copied into a dedicated folder.

    # output directory for the Python package
    set(WIN_PYPACK_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/pyohkl")
    file(MAKE_DIRECTORY "${WIN_PYPACK_OUTDIR}")

    # Package init file
    configure_file("${CMAKE_SOURCE_DIR}/swig/pypackage/__init__.py.in"
        "${WIN_PYPACK_OUTDIR}/__init__.py" @ONLY)

    message(STATUS "SWIG: Package output dir = '${WIN_PYPACK_OUTDIR}'")

    # shared library
    set(pylib_file "$<TARGET_FILE_NAME:pyohkl>")
    set(pylib_org "$<TARGET_FILE_DIR:pyohkl>/${pylib_file}")
    set(pylib_dst "${WIN_PYPACK_OUTDIR}/${pylib_file}")

    # Python wrapper
    set(pywrap_file "pyohkl.py")
    set(pywrap_org "${CMAKE_SWIG_OUTDIR}/${pywrap_file}")
    set(pywrap_dst "${WIN_PYPACK_OUTDIR}/${pywrap_file}")

    add_custom_command(
        TARGET pyohkl
        POST_BUILD
        # copy the shared library (.pyd)
        COMMAND ${CMAKE_COMMAND} -E copy "${pylib_org}" "${pylib_dst}"
        COMMAND ${CMAKE_COMMAND} -E echo
        "-- [Windows] SWIG: Copied shared library '${pylib_org}' => '${pylib_dst}'"
        # copy the Python wrapper (.py)
        COMMAND ${CMAKE_COMMAND} -E copy "${pywrap_org}" "${pywrap_dst}"
        COMMAND ${CMAKE_COMMAND} -E echo
        "-- [Windows] SWIG: Copied Python wrapper '${pywrap_org}' => '${pywrap_dst}'")
else()
    # Python wrapper
    configure_file("${CMAKE_SOURCE_DIR}/swig/pypackage/__init__.py.in"
        "${CMAKE_SWIG_OUTDIR}/__init__.py" @ONLY)
endif(WIN32)
