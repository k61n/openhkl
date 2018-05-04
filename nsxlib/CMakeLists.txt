GET_SUBDIRECTORIES(NSXLIB_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NSXLIB_SUBDIRS}
                    ${CMAKE_BINARY_DIR}/include/nsxlib)

# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE NSXLIB_SOURCES *.cpp *.c)
file(GLOB_RECURSE NSXLIB_HEADERS *.h)

# Get all the headers (raw + generated ones)
file(GLOB NSXLIB_ALL_HEADERS ${CMAKE_BINARY_DIR}/include/nsxlib/*.h)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/copy-nsxlib-headers.cmake "
  foreach (path ${NSXLIB_HEADERS})
    get_filename_component(filename \${path} NAME)
    configure_file(\${path} ${CMAKE_BINARY_DIR}/include/nsxlib/\${filename} COPYONLY)
  endforeach ()")

# Creating the library
# Under Windows and Miscrosoft Visual Studio the library will be static to avoid
# the DLL_EXPORT mess
if(MSVC OR NSX_BUILD_STATIC)
  add_library(nsx STATIC ${NSXLIB_SOURCES} ${NSXLIB_ALL_HEADERS})
else()
  add_library(nsx SHARED ${NSXLIB_SOURCES} ${NSXLIB_ALL_HEADERS})
endif()

add_custom_target(copy-nsx-headers COMMAND ${CMAKE_COMMAND} -P copy-nsxlib-headers.cmake)
add_dependencies(copy-nsx-headers generate_databases_resources)
add_dependencies(copy-nsx-headers generate_instruments_resources)
add_dependencies(nsx copy-nsx-headers)

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
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/nsxlib/ DESTINATION include/nsxlib FILES_MATCHING PATTERN "*.h")

if(NSX_TIDY)
  add_tidy_target(nsx)
endif()
