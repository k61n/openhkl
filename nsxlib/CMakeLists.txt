
include_directories(
  ${CMAKE_BINARY_DIR}/include/
)

# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE sources *.cpp *.c)
file(GLOB_RECURSE headers *.h)

foreach (path ${headers})
    get_filename_component(name ${path} NAME)
    configure_file(${path} ${CMAKE_BINARY_DIR}/include/${name} COPYONLY)
    set(NSXLIB_HEADERS ${NSXLIB_HEADERS} ${CMAKE_BINARY_DIR}/include/${name})
endforeach ()

# Creating the library
# Under Windows and Miscrosoft Visual Studio the library will be static to avoid
# the DLL_EXPORT mess
if(MSVC OR NSX_BUILD_STATIC)
  add_library(nsx STATIC ${sources} ${NSXLIB_HEADERS})
else()
  add_library(nsx SHARED ${sources} ${NSXLIB_HEADERS})
endif()

target_link_libraries(nsx 
    ${HDF5_LIBRARIES}
    blosc_static
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${YAML_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES}
    )

# Install section, installing libraries
install(TARGETS nsx DESTINATION lib)
install(CODE "MESSAGE(\"Installing headers...\")")
install(FILES ${headers} DESTINATION nsxlib)

if(NSX_TIDY)
  add_tidy_target(nsx)
endif()
