###################################################################################################
#  Python scripts that require a path to build/swig
###################################################################################################

if(NSX_PYTHON)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/workflow.py workflow.py)
endif()
