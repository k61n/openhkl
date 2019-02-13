if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

include(MakeResources)

get_subdirectories(NSXLIB_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE NSXLIB_SOURCES *.cpp *.c)

# Get the nsxlib headers
file(GLOB_RECURSE NSXLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

include_directories(${NSXLIB_SUBDIRS}
                    ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_library(${NSX_LIBRARY} ${NSX_LIB_TYPE} ${NSXLIB_SOURCES} ${NSXLIB_RESOURCES_CPP})

target_link_libraries(${NSX_LIBRARY} 
    ${HDF5_LIBRARIES}
    blosc_static
    ${Boost_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${YAML_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/copy-nsxlib-headers.cmake
    "foreach (path ${NSXLIB_HEADERS} ${NSXLIB_GENERATED_HEADERS})
         get_filename_component(filename \${path} NAME)
         configure_file(\${path} ${CMAKE_CURRENT_BINARY_DIR}/include/nsxlib/\${filename} COPYONLY)
     endforeach()")
add_custom_target(copy-nsx-headers COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/copy-nsxlib-headers.cmake)
add_dependencies(copy-nsx-headers generate-nsx-resources)

add_dependencies(${NSX_LIBRARY} copy-nsx-headers generate-nsx-resources)

if(ENABLE_CPACK)
    if(APPLE)
        set_target_properties(${NSX_LIBRARY} PROPERTIES INSTALL_RPATH "@loader_path/../Frameworks;@loader_path/../lib")
    endif()
endif()

# Install section, installing libraries
install(TARGETS ${NSX_LIBRARY} DESTINATION ${NSX_INSTALL_DIR}lib COMPONENT LIB)
install(FILES ${NSXLIB_HEADERS} DESTINATION ${NSX_INSTALL_DIR}include/nsxlib COMPONENT DEV)
install(FILES ${NSXLIB_GENERATED_HEADERS} DESTINATION ${NSX_INSTALL_DIR}include/nsxlib COMPONENT DEV)

# Install the databases file
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/databases
        DESTINATION ${NSX_INSTALL_DIR}share/nsxtool
        COMPONENT DATA
        PATTERN ".git" EXCLUDE)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/instruments
        DESTINATION ${NSX_INSTALL_DIR}share/nsxtool
        COMPONENT DATA
        PATTERN ".git" EXCLUDE)

if(NSX_TIDY)
  add_tidy_target(${NSX_LIBRARY})
endif()
