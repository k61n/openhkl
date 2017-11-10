include_directories(${CMAKE_BINARY_DIR}/include)

file(GLOB_RECURSE test_sources "*.cpp")
file(GLOB_RECURSE test_headers "*.h")

foreach(source_file ${test_sources})
  get_filename_component(base_name ${source_file} NAME_WE)
  add_executable(${base_name} ${source_file} ${test_headers})
  target_link_libraries(${base_name} ${Boost_LIBRARIES} nsx)

  if(${NSX_VALGRIND})
    add_test(
      NAME ${base_name}
      COMMAND valgrind --error-exitcode=3 --leak-check=full --suppressions=${CMAKE_CURRENT_BINARY_DIR}/suppressions --gen-suppressions=all $<TARGET_FILE:${base_name}>
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
  else()
    add_test(${base_name} ${base_name})
  endif()

  if(NSX_TIDY)
    add_tidy_target(${base_name})
  endif()

endforeach(source_file)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

configure_file(../resources/databases/isotopes.yml isotopes.yml COPYONLY)

configure_file(crystal/lattices.dat lattices.dat COPYONLY)
configure_file(crystal/niggli_lattices.dat niggli_lattices.dat COPYONLY)
configure_file(crystal/CsOsO_15K.raf CsOsO_15K.raf COPYONLY)
configure_file(crystal/CsOsO_15K.lpt CsOsO_15K.lpt COPYONLY)

configure_file(data/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(data/D9_ascii_example D9_ascii_example COPYONLY)
configure_file(data/D9_QSCAN D9_QSCAN COPYONLY)

configure_file(data/gal3.hdf gal3.hdf COPYONLY)
configure_file(data/H5_example.hdf H5_example.hdf COPYONLY)
configure_file(data/blank.hdf blank.hdf COPYONLY)

configure_file(instrument/RotAxis.xml RotAxis.xml COPYONLY)

configure_file(instrument/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(instrument/Instrument.xml Instrument.xml COPYONLY)

configure_file(crystal/crystal_database.csv crystal_database.csv COPYONLY)
configure_file(crystal/crystallography.tsv crystallography.tsv COPYONLY)
configure_file(crystal/714898.hdf 714898.hdf COPYONLY)

# valgrind suppressions
configure_file(valgrind/suppressions suppressions COPYONLY)


# add python tests
if(NSX_PYTHON)

  SET (CTEST_ENVIRONMENT
  "PYTHONPATH=${CMAKE_BINARY_DIR}/swig"
  )
 
  file(GLOB_RECURSE python_tests *.py)

  foreach(python_script ${python_tests})
    get_filename_component(base_name ${python_script} NAME)
    add_test(NAME ${base_name} COMMAND ${PYTHON_EXECUTABLE} ${python_script})
    set_tests_properties(${base_name} PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/swig")
  endforeach(python_script)

endif(NSX_PYTHON)
