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
#  Long test data files must have been downloaded to directory NSX_TESTDATA_DIR
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
    CrChiA.nsx)

set(NSX_TESTDATA_URL https://computing.mlz-garching.de/download/testdata/nsx)
set(NSX_NIGHTLY_TESTDATA_URL ${NSX_TESTDATA_URL}/nightly)

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

# SHA384 sum
check_file(90d4b60a2ae44d2f24865b29d068569b7348ebcd61ca2333476c6daabec1cd17688273acc2bf456ebdafbbc84dee114c  CrChiA_c01runab_28603.raw)
check_file(78d39040354a03fa528aed3495b115534e2e57ec6e80614978d50122f28d28fc0a31afccf136ddc2745e5dfce717ae21  CrChiA_c01runab_28604.raw)
check_file(5679edddac02dd7e6d64dea7c049a66ad0cfe93153e73d1954bab445fa741027bdcfe3db619b21b1c41cdc43f602c4d6  CrChiA_c01runab_28605.raw)
check_file(1863ece250d26c6420e46ba0b1b1fa315951a860c544dbd9cc54cd3242549325e39fb8b22194e0e3bfccb7bfd52c7734  CrChiA_c01runab_28606.raw)
check_file(aa11cbf96d85ab6495fd2081ed469183fcde01b4f0e0f8d69d860d21095a687de54324d97a6cf360e00dd6f770c4865a  CrChiA_c01runab_28607.raw)
check_file(d27e6a2f186489034475e5900de0109ab363b956a893834e2231c5772d83cf98c9af67cae2696cbc2815191707a02887  CrChiA_c01runab_28608.raw)
check_file(ece8e0d4d63baebfdbb8222e69f505af4c37bd5aae0ee11f503106bc006910ac2edbf989797b5ee8a306ecb96acbad89  CrChiA_c01runab_28609.raw)
check_file(d5b510907b9fee32c6bccbeb1c36f000fb84d6a70fd9ba3a465bd43a0888e7a2ff09094b2d86d1165cbeba857603a746  CrChiA_c01runab_28610.raw)
check_file(edcf6b480c45f371c25d818b7e17fd63fd65fc52c61ce6afb99c6fe8e0fffcd60c82aad8ce40570dbf0965d22e2fc1d4  CrChiA_c01runab_28611.raw)
check_file(20599e6d3467b8b4bdc4866bf5dd8371220cb8fd91e4c4b36e84d85ad795fc9c1d1be0b5f712010625ceb80dd2c3c5d1  CrChiA_c01runab_28612.raw)
check_file(098b57377b2abf89d19b5da1f3489d047f7412b7f1ceb889c3279da756a540a22aafeb31923e001b92a6d4cb6ea62be6  CrChiA_c01runab_28613.raw)
check_file(a9da31c093bf1e9984a6706a100cf1c64908c9387080f7103c5a8861ede6185fab3f1ad6429cff457940c5b369b96d9e  CrChiA_c01runab_28614.raw)
check_file(fa3af6170b7006016fa5e4027062235526d47646da5ad0f00a5a4e69a3ce3e13daf929ea5770bdedb287a5a6e0852aa8  CrChiA_c01runab_28615.raw)
check_file(b82da648c43aff3b196f3e66b33150d7087071a0de72f7a79528c37886e7073c3ced38079b98a56d1e1860904c49387f  CrChiA_c01runab_28616.raw)
check_file(598d2406631dd96a6c3b0bf4fe6f41a6b7435077b9475da0178cab34649be4b548fff182fb7d2ffed34655052d042282  CrChiA_c01runab_28617.raw)
check_file(e99b5717cfd1e7d8bb61bba900be2961163c3ca2ccb246a8b9a8012e32859eed5a76cdf1433e4b0b7a30fbba9d15ac66  CrChiA.nsx)

###################################################################################################
#  Simulated data files for full workflow test (NSX_FULL_WORKFLOW_TEST)
###################################################################################################
if (NSX_FULL_WORKFLOW_TEST)
    message(STATUS "Full workflow test enabled, checking for data file...")
    set(nightly_test_file small_cell_low_intensity.tar.gz)
    file(DOWNLOAD ${NSX_NIGHTLY_TESTDATA_URL}/${nightly_test_file} ${NSX_TESTDATA_DIR}/${nightly_test_file})
    check_file(f98e6ae071c6d89c122b89ac9880fa85497ed1f552e524933c37fe70adce30066dd4b37ce0ed660484d0892f8b1c62e3  small_cell_low_intensity.tar.gz)
    file(ARCHIVE_EXTRACT INPUT ${nightly_test_file})
endif()
