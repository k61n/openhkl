###################################################################################################
#  Python scripts that require a path to build/swig
###################################################################################################

if(NSX_PYTHON)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/workflow.py workflow.py)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/integrate.py integrate.py)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/pxrange_test.py pxrange_test.py)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/frange_test.py frange_test.py)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/peakend_test.py peakend_test.py)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/bootstrap.py bootstrap.py)
  configure_file(${CMAKE_SOURCE_DIR}/scripts/qspace-ellipsoid.py qspace-ellipsoid.py)
endif()
