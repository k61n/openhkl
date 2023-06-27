# Hardware parameter files
add_subdirectory(data/instruments)
file(GLOB hardware_parameter_yml "${CMAKE_SOURCE_DIR}/data/instruments/*.yml")
foreach(file ${hardware_parameter_yml})
  get_filename_component(file_name ${file} NAME)
  configure_file(${file} ${file_name} COPYONLY)
endforeach()
