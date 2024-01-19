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
    CrChiA.ohkl
    Trypsin.ohkl)

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
