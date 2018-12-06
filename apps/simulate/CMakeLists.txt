add_executable(simulate simulate.cpp)
target_include_directories(
    simulate
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${NSXQT_SUBDIRS}
    ${CMAKE_BINARY_DIR}/nsxlib/include)

target_link_libraries(simulate nsx ${HDF5_LIBRARIES})

if(NSX_TIDY)
  add_tidy_target(simulate)
endif()

# test that simulation runs correctly
add_test(NAME simulate COMMAND simulate ${CMAKE_SOURCE_DIR}/tests/data/blank.hdf simulated.hdf)
