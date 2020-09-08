configure_file(../cpp/crystal/lattices.dat lattices.dat COPYONLY)
configure_file(../cpp/crystal/niggli_lattices.dat niggli_lattices.dat COPYONLY)
configure_file(../cpp/crystal/CsOsO_15K.raf CsOsO_15K.raf COPYONLY)
configure_file(../cpp/crystal/CsOsO_15K.lpt CsOsO_15K.lpt COPYONLY)

configure_file(../cpp/data/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(../cpp/data/D9_ascii_example D9_ascii_example COPYONLY)
configure_file(../cpp/data/D9_QSCAN D9_QSCAN COPYONLY)

configure_file(../cpp/data/gal3.hdf gal3.hdf COPYONLY)
configure_file(../cpp/data/d19_test.hdf d19_test.hdf COPYONLY)
configure_file(../cpp/data/H5_example.hdf H5_example.hdf COPYONLY)
configure_file(../cpp/data/blank.hdf blank.hdf COPYONLY)

configure_file(../cpp/geometry/convex_hull_faces.xyz convex_hull_faces.xyz COPYONLY)
configure_file(../cpp/geometry/convex_hull_vertices.xyz convex_hull_vertices.xyz COPYONLY)

configure_file(../cpp/instrument/RotAxis.xml RotAxis.xml COPYONLY)

configure_file(../cpp/instrument/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(../cpp/instrument/Instrument.xml Instrument.xml COPYONLY)

configure_file(../cpp/crystal/crystal_database.csv crystal_database.csv COPYONLY)
configure_file(../cpp/crystal/crystallography.tsv crystallography.tsv COPYONLY)
configure_file(../cpp/crystal/714898.hdf 714898.hdf COPYONLY)

# Dealing with large test data files -- zamaan
set(test_data_url http://apps.jcns.fz-juelich.de/datasamples/nsxtool/BioDiff/FutA/crystal9)
set(test_data_dir test/data)
set(test_files p16106_00029299.tiff 
               p16106_00029300.tiff 
               p16106_00029301.tiff 
               p16106_00029302.tiff 
               p16106_00029303.tiff 
               p16106_00029304.tiff 
               p16106_00029305.tiff
               FutA.nsx
               FutA-indexed.nsx)
foreach(test_file ${test_files})
  if(NOT EXISTS ${test_data_dir}/${test_file})
    message(STATUS "Downloading test data ${test_data_url}/${test_file}")
    file(DOWNLOAD ${test_data_url}/${test_file} ${test_data_dir}/${test_file})
  endif()
endforeach()
