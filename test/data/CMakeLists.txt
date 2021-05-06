###################################################################################################
#  Short test data files are part of the source tree
###################################################################################################

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
configure_file(../cpp/data/soak_9_d2_I_scanI_9483.x soak_9_d2_I_scanI_9483.x COPYONLY)

configure_file(../cpp/geometry/convex_hull_faces.xyz convex_hull_faces.xyz COPYONLY)
configure_file(../cpp/geometry/convex_hull_vertices.xyz convex_hull_vertices.xyz COPYONLY)

configure_file(../cpp/instrument/RotAxis.xml RotAxis.xml COPYONLY)

configure_file(../cpp/instrument/D10_ascii_example D10_ascii_example COPYONLY)
configure_file(../cpp/instrument/Instrument.xml Instrument.xml COPYONLY)

configure_file(../cpp/crystal/crystal_database.csv crystal_database.csv COPYONLY)
configure_file(../cpp/crystal/crystallography.tsv crystallography.tsv COPYONLY)
configure_file(../cpp/crystal/714898.hdf 714898.hdf COPYONLY)

###################################################################################################
#  Long test data files must have been downloaded to directory NSX_TESTDATA_DIR
###################################################################################################

set(test_files
    p16106_00029299.tiff
    p16106_00029300.tiff
    p16106_00029301.tiff
    p16106_00029302.tiff
    p16106_00029303.tiff
    p16106_00029304.tiff
    p16106_00029305.tiff
    FutA.nsx)

set(NSX_TESTDATA_URL https://computing.mlz-garching.de/download/testdata/nsx)

function(check_file expected_hash fname)
    if(NOT EXISTS ${NSX_TESTDATA_DIR}/${fname})
        message(STATUS "File ${fname} not found, downloading...")
        file(DOWNLOAD ${NSX_TESTDATA_URL}/${fname} ${NSX_TESTDATA_DIR}/${fname})
    else()
        file(SHA384 ${NSX_TESTDATA_DIR}/${fname} found_hash)
        if(NOT (${found_hash} STREQUAL ${expected_hash}))
            message(STATUS "- File ${fname} corrupted, downloading...")
            file(DOWNLOAD ${NSX_TESTDATA_URL}/${fname} ${NSX_TESTDATA_DIR}/${fname})
        else()
            if (NOT ${NSX_TESTDATA_DIR} STREQUAL "test/data")
                configure_file(${NSX_TESTDATA_DIR}/${fname} ${fname} COPYONLY)
            endif()
        endif()
    endif()
endfunction()

list(LENGTH test_files nfiles)
message(STATUS
    "Checking ${nfiles} test data files in directory NSX_TESTDATA_DIR=${NSX_TESTDATA_DIR}:")
check_file(9b10bb9042b0026e4b2a725913c1d60807ea31cf21a6c6c83fed741755461ec4166664bfdc16a098936815471ceb74b1 FutA.nsx)
check_file(6f9c5b7e053a7f1e8b30f7b3ee39876e96fc1c70e6f1ebd5db61d35395e87b44426aa87cd0735a795179395c6e426742  p16106_00029299.tiff)
check_file(bcb8d97ffba00141a13025d476349d3d6849fc6f4ede5eb7d5c3bc830c717097869fb65fbb704034d4e85abed16fc5b9  p16106_00029300.tiff)
check_file(b7634e3f97de7a3532d0dc4f11c1acba897fac209b59bcb4ef0c4a07523a760486cdb31009e69d3c831713e9170698b2  p16106_00029301.tiff)
check_file(d7551c196a513853575f5f79f755e64cfa263fc1cb381780e86fc3d6c5d8b531e77e0d055fc88dad0bfb6103e4748b46  p16106_00029302.tiff)
check_file(b44f01e8154c91e8ff821c0818982e1643931f9f124e2a7463cd0b940f06c9312f570ac2c80715cd519ddd7a92e6bd87  p16106_00029303.tiff)
check_file(4752be16363af29ff6066879f990f4f0d29cb8a59e1c8328cf1c569c82cc471982354b712970ea1f120ca855b2d48224  p16106_00029304.tiff)
check_file(ad70d1b3adbdaa94dc96070daa7b206e5114097009b9dc0b2fa8cec9d0fcc5b66d9b2144ef76b93bbfe1e52ccd275f61  p16106_00029305.tiff)
