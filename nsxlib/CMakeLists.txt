

include_directories(${CMAKE_BINARY_DIR}/include/
                    chemistry
                    data
                    geometry
                    kernel
                    imaging
                    integration/include
                    materials/include
                    utils
                    crystal
                    instrument
                    ../externals/c-blosc/blosc)

file(GLOB_RECURSE sources *.cpp *.c)

file(GLOB_RECURSE headers *.h)

# Creating the library
# Under Windows and Miscrosoft Visual Studio the library will be static to avoid
# the DLL_EXPORT mess
if(MSVC)
    add_library(NSXTool STATIC ${sources} ${headers})
else()
    add_library(NSXTool SHARED ${sources} ${headers})
endif()




# Link
#target_link_libraries(NSXTool blosc_shared ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${FFTW3_LIBRARIES} ${HDF5_LIBRARIES} ${TIFF_LIBRARY})
target_link_libraries(NSXTool ${HDF5_LIBRARIES} blosc_shared ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${FFTW3_LIBRARIES} ${TIFF_LIBRARY})


# Install section, installing libraries 
install(TARGETS NSXTool DESTINATION lib)
set(INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/NSXTool/")
file(GLOB_RECURSE all_headers *.h)
install(CODE "MESSAGE(\"Installing headers...\")")
install(FILES ${headers} DESTINATION ${INCLUDE_DIR})
