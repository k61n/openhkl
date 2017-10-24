# Autogenerate a .i file from doxygen comments

function(GeneratePythonDocs PD_TARGET SWIG_DIR)
    add_custom_command(
        OUTPUT ${PD_TARGET}
        COMMAND ${PYTHON_EXECUTABLE} ${SWIG_DIR}/doxy2swig.py ${CMAKE_BINARY_DIR}/docs/xml/index.xml ${PD_TARGET}
        DEPENDS ${CMAKE_BINARY_DIR}/docs/xml/index.xml
        )
    add_custom_target(python-docs ALL DEPENDS ${PD_TARGET})
endfunction()
