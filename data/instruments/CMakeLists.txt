# Instrument parameter files
file(GLOB instrument_parameter_yml *.yml)
foreach(file ${instrument_parameter_yml})
  get_filename_component(file_name ${file} NAME)
  configure_file(${file} ${file_name} COPYONLY)
endforeach()
