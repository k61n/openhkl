file(GLOB_RECURSE sources *.cpp)

add_library(${NSX_LIBRARY} ${sources})

target_include_directories(
    ${NSX_LIBRARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR})

target_link_libraries(${NSX_LIBRARY}
    PUBLIC
    nsxbase
    nsxtables
    PRIVATE
    ${HDF5_LIBRARIES}
    ${BLOSC_LIBRARY}
    ${Boost_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES})
