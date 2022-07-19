set(lib openhklcore)

file(GLOB_RECURSE sources *.cpp)

add_library(${lib} ${sources})

target_include_directories(${lib}
    PUBLIC
    ${CMAKE_BINARY_DIR} # for manifest.h
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    )

target_link_libraries(${lib}
    PUBLIC
    openhklbase
    openhkltables
    PRIVATE
    ${HDF5_LIBRARIES}
    ${Blosc_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES}
    )
