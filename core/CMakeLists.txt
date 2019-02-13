if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

include(MakeResources)

get_subdirectories(core_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE core_SOURCES *.cpp *.c)

# Get the core headers
file(GLOB_RECURSE core_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

include_directories(${core_SUBDIRS}
                    ${CMAKE_CURRENT_BINARY_DIR}/generated)

add_library(${NSX_LIBRARY} ${NSX_LIB_TYPE} ${core_SOURCES} ${core_RESOURCES_CPP})

target_link_libraries(${NSX_LIBRARY} 
    ${HDF5_LIBRARIES}
    blosc_static
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
add_dependencies(copy-nsx-headers generate-nsx-resources)

add_dependencies(${NSX_LIBRARY} copy-nsx-headers generate-nsx-resources)

if(ENABLE_CPACK)
    if(APPLE)
        set_target_properties(${NSX_LIBRARY} PROPERTIES INSTALL_RPATH "@loader_path/../Frameworks;@loader_path/../lib")
    endif()
endif()

# Install section, installing libraries
install(TARGETS ${NSX_LIBRARY} DESTINATION ${NSX_INSTALL_DIR}lib COMPONENT LIB)
install(FILES ${core_HEADERS} DESTINATION ${NSX_INSTALL_DIR}include/core COMPONENT DEV)
install(FILES ${core_GENERATED_HEADERS} DESTINATION ${NSX_INSTALL_DIR}include/core COMPONENT DEV)

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
