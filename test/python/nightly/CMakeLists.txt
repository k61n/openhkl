set(CTEST_ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/swig")

if (OHKL_FULL_WORKFLOW_TEST)
    file(GLOB python_tests *.py)
    foreach(python_script ${python_tests})
        get_filename_component(base_name ${python_script} NAME)
        add_test(NAME ${base_name} COMMAND ${PYTHON_EXECUTABLE} ${python_script}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data)
        set_tests_properties(${base_name} PROPERTIES
            ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/swig"
            LABELS "nightly")
    endforeach(python_script)
endif()
