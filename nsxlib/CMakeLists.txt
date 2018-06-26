include(MakeResources)

get_subdirectories(NSXLIB_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Set up the Version file
configure_file(${CMAKE_SOURCE_DIR}/nsxlib/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/Version.h @ONLY)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/generated/Version.h PROPERTIES GENERATED TRUE)

# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE NSXLIB_SOURCES *.cpp *.c)

# Get the nsxlib headers
file(GLOB_RECURSE NSXLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

include_directories(${NSXLIB_SUBDIRS} ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_library(${NSX_LIBRARY} ${NSX_LIB_TYPE} ${NSXLIB_SOURCES} ${NSXLIB_RESOURCES_CPP})

target_link_libraries(${NSX_LIBRARY} 
    ${HDF5_LIBRARIES}
    blosc_static
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${YAML_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES}
    )

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/copy-nsxlib-headers.cmake
    "foreach (path ${NSXLIB_HEADERS} ${NSXLIB_GENERATED_HEADERS})
         get_filename_component(filename \${path} NAME)
         configure_file(\${path} ${CMAKE_CURRENT_BINARY_DIR}/include/nsxlib/\${filename} COPYONLY)
     endforeach()")
add_custom_target(copy-nsx-headers COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/copy-nsxlib-headers.cmake)
add_dependencies(copy-nsx-headers generate-nsx-resources)

add_dependencies(${NSX_LIBRARY} copy-nsx-headers generate-nsx-resources)

# Install section, installing libraries
install(TARGETS ${NSX_LIBRARY} DESTINATION lib COMPONENT Libraries)
install(FILES ${NSXLIB_HEADERS} DESTINATION include/nsxlib COMPONENT Headers)
install(FILES ${NSXLIB_GENERATED_HEADERS} DESTINATION include/nsxlib COMPONENT Headers)

# Install the databases file
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/databases
        DESTINATION share/nsxtool
        COMPONENT Data
        PATTERN ".git" EXCLUDE)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/instruments
        DESTINATION share/nsxtool
        COMPONENT Data
        PATTERN ".git" EXCLUDE)

if(NSX_TIDY)
  add_tidy_target(${NSX_LIBRARY})
endif()
