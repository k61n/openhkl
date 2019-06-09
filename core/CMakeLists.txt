# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE core_SOURCES *.cpp *.c)

file(GLOB_RECURSE core_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${NSX_LIBRARY} ${NSX_LIB_TYPE} ${core_SOURCES})

target_include_directories(
    ${NSX_LIBRARY}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/generated)

target_link_libraries(${NSX_LIBRARY}
    PUBLIC
    ${HDF5_LIBRARIES}
    ${BLOSC_LIBRARY}
    ${Boost_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${YAML_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/copy-core-headers.cmake
    "foreach (path ${core_HEADERS} ${core_GENERATED_HEADERS})
         get_filename_component(filename \${path} NAME)
         configure_file(\${path} ${CMAKE_CURRENT_BINARY_DIR}/include/core/\${filename} COPYONLY)
     endforeach()")
add_custom_target(copy-nsx-headers COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/copy-core-headers.cmake)

# Install section, installing libraries
install(TARGETS ${NSX_LIBRARY} DESTINATION ${NSX_INSTALL_DIR}lib COMPONENT LIB)
install(FILES ${core_HEADERS} DESTINATION ${NSX_INSTALL_DIR}include/core COMPONENT DEV)
install(FILES ${core_GENERATED_HEADERS} DESTINATION ${NSX_INSTALL_DIR}include/core COMPONENT DEV)

if(NSX_TIDY)
    add_tidy_target(${NSX_LIBRARY})
endif()
