###################################################################################################
#  Python scripts that require a path to build/swig
###################################################################################################

if (OHKL_PYTHON)
  file(GLOB python_scripts *.py)

  foreach(python_script ${python_scripts})
    get_filename_component(base_name ${python_script} NAME)

    configure_file(${CMAKE_SOURCE_DIR}/scripts/${base_name} ${base_name})
  endforeach(python_script)
endif()
