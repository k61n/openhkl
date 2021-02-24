set(lib nsxcore)

file(GLOB_RECURSE sources *.cpp)

add_library(${lib} ${sources})

target_include_directories(${lib}
    PUBLIC
    ${XSection_INCLUDE_DIR}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    )

target_link_libraries(${lib}
    PUBLIC
    nsxbase
    nsxtables
    PRIVATE
    ${HDF5_LIBRARIES}
    ${Blosc_LIBRARY}
    ${Boost_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES}
    ${XSection_LIBRARIES}
    ${Qt5Core_LIBRARIES}
    )
