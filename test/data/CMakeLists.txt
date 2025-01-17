###################################################################################################
#  Short test data files are part of the source tree
###################################################################################################

file(GLOB unit_test_files "${CMAKE_SOURCE_DIR}/test/data/unit/*")
foreach(file ${unit_test_files})
    get_filename_component(file_name ${file} NAME)
    configure_file(${file} ${file_name} COPYONLY)
endforeach()

###################################################################################################
#  Larger files for functional tests
###################################################################################################

set(test_files
    CrChiA_c01runab_28603.raw
    CrChiA_c01runab_28604.raw
    CrChiA_c01runab_28605.raw
    CrChiA.ohkl
    det_pos1_001.dat
    det_pos1_002.dat
    det_pos1_003.dat
    p11202_00009983.tiff
    p11202_00009984.tiff
    p11202_00009985.tiff
    p11202_00009986.tiff
    p11202_00009987.tiff
    p11202_00009988.tiff
    p11202_00009989.tiff
    p11202_00009990.tiff
    p11202_00009991.tiff
    p11202_00009992.tiff
    p11202_00009993.tiff
    p11202_00009994.tiff
    p11202_00009995.tiff
    p11202_00009996.tiff
    p11202_00009997.tiff
    p11202_00009998.tiff
    p11202_00009999.tiff
    p11202_00010000.tiff
    p11202_00010001.tiff
    p11202_00010002.tiff
    509192.nxs
    509193.nxs
    509194.nxs
    Trypsin.ohkl
    Trypsin-pxsum.ohkl)

  list(LENGTH test_files nfiles)
  message(STATUS
    "Checking ${nfiles} test data files in directory ${CMAKE_BINARY_DIR}/${OHKL_TESTDATA_DIR}:")

  foreach(file ${test_files})
    if(NOT EXISTS ${CMAKE_BINARY_DIR}/${OHKL_TESTDATA_DIR}/${file})
      message(STATUS "File ${file} not found, downloading...")
      execute_process(COMMAND git lfs pull --include ${file})
      configure_file(${CMAKE_SOURCE_DIR}/test/data/${file} ${file} COPYONLY)
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
        if(NOT EXISTS ${CMAKE_BINARY_DIR}/${OHKL_TESTDATA_DIR}/${file})
            message(STATUS "File ${file} not found, downloading...")
            execute_process(COMMAND git lfs pull --include "${file}")
            configure_file(${CMAKE_SOURCE_DIR}/test/data/${file} ${file} COPYONLY)
        else()
            if (NOT ${OHKL_TESTDATA_DIR} STREQUAL "test/data")
                configure_file(${OHKL_TESTDATA_DIR}/${file} ${file} COPYONLY)
            endif()
        endif()
        file(ARCHIVE_EXTRACT INPUT ${file})
    endforeach()
endif()
