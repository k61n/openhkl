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
add_subdirectory(chemistry)
add_subdirectory(crystal)
add_subdirectory(data)
add_subdirectory(geometry)
add_subdirectory(imaging)
add_subdirectory(instrument)
add_subdirectory(kernel)
add_subdirectory(utils)


