# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE core_SOURCES *.cpp *.c)

add_library(${NSX_LIBRARY} ${NSX_LIB_TYPE} ${core_SOURCES})

target_include_directories(
    ${NSX_LIBRARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR})

target_link_libraries(${NSX_LIBRARY}
    PRIVATE
    ${HDF5_LIBRARIES}
    ${BLOSC_LIBRARY}
    ${Boost_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${YAML_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES})
