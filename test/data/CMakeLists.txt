###################################################################################################
#  Short test data files are part of the source tree
###################################################################################################

configure_file(../cpp/crystal/lattices.dat lattices.dat COPYONLY)
configure_file(../cpp/crystal/niggli_lattices.dat niggli_lattices.dat COPYONLY)
configure_file(../cpp/crystal/CsOsO_15K.raf CsOsO_15K.raf COPYONLY)
configure_file(../cpp/crystal/CsOsO_15K.lpt CsOsO_15K.lpt COPYONLY)

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

configure_file(../cpp/instrument/Instrument.xml Instrument.xml COPYONLY)

configure_file(../cpp/crystal/crystal_database.csv crystal_database.csv COPYONLY)
configure_file(../cpp/crystal/crystallography.tsv crystallography.tsv COPYONLY)
configure_file(../cpp/crystal/714898.hdf 714898.hdf COPYONLY)

###################################################################################################
#  Larger files for functional tests
###################################################################################################

set(test_files
    CrChiA_c01runab_28603.raw
    CrChiA_c01runab_28604.raw
    CrChiA_c01runab_28605.raw
    CrChiA_c01runab_28606.raw
    CrChiA_c01runab_28607.raw
    CrChiA_c01runab_28608.raw
    CrChiA_c01runab_28609.raw
    CrChiA_c01runab_28610.raw
    CrChiA_c01runab_28611.raw
    CrChiA_c01runab_28612.raw
    CrChiA_c01runab_28613.raw
    CrChiA_c01runab_28614.raw
    CrChiA_c01runab_28615.raw
    CrChiA_c01runab_28616.raw
    CrChiA_c01runab_28617.raw
    CrChiA.ohkl)

list(LENGTH test_files nfiles)
message(STATUS
    "Checking ${nfiles} test data files in directory OHKL_TESTDATA_DIR=${OHKL_TESTDATA_DIR}:")

foreach(file ${test_files})
    if(NOT EXISTS ${OHKL_TESTDATA_DIR}/${file})
        message(STATUS "File ${file} not found, downloading...")
        execute_process(COMMAND git lfs pull --include ${file})
        configure_file(../data/${file} ${file} COPYONLY)
    else()
          if (NOT ${OHKL_TESTDATA_DIR} STREQUAL "test/data")
              configure_file(${OHKL_TESTDATA_DIR}/${file} ${file} COPYONLY)
          endif()
      endif()
endforeach()

###################################################################################################
#  Data files for full workflow test (OHKL_FULL_WORKFLOW_TEST)
###################################################################################################

set(nightly_test_files
  small_cell_low_intensity.tar.gz
  trypsin.tar.gz)

if (OHKL_FULL_WORKFLOW_TEST)
    message(STATUS "Full workflow tests enabled, checking for data files...")

    foreach(file ${nightly_test_files})
        if(NOT EXISTS ${OHKL_TESTDATA_DIR}/${file})
            message(STATUS "File ${file} not found, downloading...")
            execute_process(COMMAND git lfs pull --include ${file})
            configure_file(../data/${file} ${file} COPYONLY)
        else()
            if (NOT ${OHKL_TESTDATA_DIR} STREQUAL "test/data")
                configure_file(${OHKL_TESTDATA_DIR}/${file} ${file} COPYONLY)
            endif()
        endif()
        file(ARCHIVE_EXTRACT INPUT ${file})
    endforeach()
endif()
