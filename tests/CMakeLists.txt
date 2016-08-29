include_directories(
    ${CMAKE_SOURCE_DIR}/nsxlib/chemistry/
    ${CMAKE_SOURCE_DIR}/nsxlib/crystal/
    ${CMAKE_SOURCE_DIR}/nsxlib/data/
    ${CMAKE_SOURCE_DIR}/nsxlib/geometry/
    ${CMAKE_SOURCE_DIR}/nsxlib/imaging/
    ${CMAKE_SOURCE_DIR}/nsxlib/instrument/
    ${CMAKE_SOURCE_DIR}/nsxlib/kernel/
    ${CMAKE_SOURCE_DIR}/nsxlib/utils/
)


# add_subdirectory(chemistry)
# add_subdirectory(crystal)
# add_subdirectory(data)
# add_subdirectory(geometry)
# add_subdirectory(imaging)
# add_subdirectory(instrument)
# add_subdirectory(kernel)
# add_subdirectory(utils)


file(GLOB_RECURSE test_sources "*.cpp")
file(GLOB_RECURSE test_headers "*.h")

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_AUTOMOC ON)

foreach(source_file ${test_sources})
  #message("test sources is ${test_sources}")
  #message("source file is ${source_file}")
  get_filename_component(base_name ${source_file} NAME_WE)
  #message("base name is ${base_name}")
  add_executable(${base_name} ${source_file} ${test_headers})
  target_link_libraries(${base_name} ${Boost_LIBRARIES} NSXTool)
  add_test(${base_name} ${base_name})
  #message("Added test ${base_name}")
endforeach(source_file)

#add_custom_target(check ALL COMMAND ${CMAKE_CTEST_COMMAND})
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})




configure_file(../resources/databases/isotopes.xml isotopes.xml COPYONLY)
configure_file(../resources/databases/elements.xml elements.xml COPYONLY)
configure_file(../resources/databases/materials.xml materials.xml COPYONLY)



configure_file(crystal/CsOsO_15K.raf CsOsO_15K.raf COPYONLY)
configure_file(crystal/CsOsO_15K.lpt CsOsO_15K.lpt COPYONLY)



configure_file(data/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(data/D9_ascii_example D9_ascii_example COPYONLY)
configure_file(data/D9_QSCAN D9_QSCAN COPYONLY)

configure_file(data/H5_example.hdf H5_example.hdf COPYONLY)



configure_file(instrument/RotAxis.xml RotAxis.xml COPYONLY)


configure_file(instrument/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(instrument/Instrument.xml Instrument.xml COPYONLY)

configure_file(crystal/crystal_database.csv crystal_database.csv COPYONLY)
configure_file(crystal/crystallography.tsv crystallography.tsv COPYONLY)


