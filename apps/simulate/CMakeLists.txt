
# add project root directory to include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${NSXQT_SUBDIRS}
                    ${CMAKE_BINARY_DIR}/nsxlib/include)


# Build the nsxqt executable
add_executable(simulate simulate.cpp)

# Link the nsxqt executable with the Qt5 libraries in use.
target_link_libraries(simulate nsx ${HDF5_LIBRARIES})

# tidy
if(NSX_TIDY)
  add_tidy_target(simulate)
endif()

# test that simulation runs correctly
add_test(NAME simulate COMMAND simulate ${CMAKE_SOURCE_DIR}/tests/data/blank.hdf simulated.hdf)
