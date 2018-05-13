GET_SUBDIRECTORIES(NSXLIB_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

###########################################################################
# Setup resources manager helper program
###########################################################################

add_executable(make_resource ${CMAKE_SOURCE_DIR}/scripts/make_resource.cpp)
add_executable(make_resources_map ${CMAKE_SOURCE_DIR}/scripts/make_resources_map.cpp)

configure_file(${CMAKE_SOURCE_DIR}/cmake/ResourcesMap.h.in ${CMAKE_CURRENT_BINARY_DIR}/resources/ResourcesMap.h)
configure_file(${CMAKE_SOURCE_DIR}/cmake/ResourcesMap.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/tmp/ResourcesMap.cpp)

set(NSXLIB_RESOURCES_CPP ${CMAKE_CURRENT_BINARY_DIR}/resources/ResourcesMap.cpp)

file(GLOB_RECURSE NSX_RESOURCES ${CMAKE_SOURCE_DIR}/resources/*.yml)
foreach (NSX_RESOURCE ${NSX_RESOURCES})
    get_filename_component(NSX_RESOURCE_NAME ${NSX_RESOURCE} NAME_WE)
    get_filename_component(NSX_RESOURCE_DIR ${NSX_RESOURCE} DIRECTORY)
    get_filename_component(NSX_RESOURCE_TYPE ${NSX_RESOURCE_DIR} NAME)

    configure_file(${CMAKE_SOURCE_DIR}/cmake/Resource.h.in   ${CMAKE_CURRENT_BINARY_DIR}/resources/Resource${NSX_RESOURCE_NAME}.h)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/Resource.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/tmp/Resource${NSX_RESOURCE_NAME}.cpp)
    list(APPEND NSXLIB_RESOURCES_CPP ${CMAKE_CURRENT_BINARY_DIR}/resources/Resource${NSX_RESOURCE_NAME}.cpp)
    add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/resources/Resource${NSX_RESOURCE_NAME}.cpp
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tmp/Resource${NSX_RESOURCE_NAME}.cpp ${CMAKE_CURRENT_BINARY_DIR}/resources/Resource${NSX_RESOURCE_NAME}.cpp
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/make_resource ${NSX_RESOURCE} ${CMAKE_CURRENT_BINARY_DIR}/resources/Resource${NSX_RESOURCE_NAME}.cpp
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/make_resource ${CMAKE_SOURCE_DIR}/cmake/Resource.h.in
        COMMENT "Generating ${CMAKE_CURRENT_BINARY_DIR}/resources/Resource${NSX_RESOURCE_NAME}.cpp resource"
    )
endforeach()

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate-nsx-resources.cmake 
 "execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tmp/ResourcesMap.cpp ${CMAKE_CURRENT_BINARY_DIR}/resources/ResourcesMap.cpp)
  foreach (NSX_RESOURCE ${NSX_RESOURCES})
    get_filename_component(NSX_RESOURCE_NAME \${NSX_RESOURCE} NAME_WE)
    get_filename_component(NSX_RESOURCE_DIR \${NSX_RESOURCE} DIRECTORY)
    get_filename_component(NSX_RESOURCE_TYPE \${NSX_RESOURCE_DIR} NAME)

    execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/make_resources_map \${NSX_RESOURCE_TYPE} \${NSX_RESOURCE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/resources/ResourcesMap.cpp)
  endforeach ()"
)

add_custom_target(generate-nsx-resources COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/generate-nsx-resources.cmake)
add_dependencies(generate-nsx-resources make_resources_map)

set_source_files_properties(${NSXLIB_RESOURCES_CPP} PROPERTIES GENERATED TRUE)

# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE NSXLIB_SOURCES *.cpp *.c)
list(APPEND NSXLIB_SOURCES ${NSXLIB_RESOURCES_CPP})

# Get the nsxlib headers
file(GLOB_RECURSE NSXLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_BINARY_DIR}/resources/*.h)

# Get the nsxlib headers
file(GLOB NSXLIB_GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/resources/*.h)
list(APPEND NSXLIB_HEADERS ${NSXLIB_GENERATED_HEADERS})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/copy-nsxlib-headers.cmake
    "foreach (path ${NSXLIB_HEADERS})
         get_filename_component(filename \${path} NAME)
         configure_file(\${path} ${CMAKE_CURRENT_BINARY_DIR}/include/nsxlib/\${filename} COPYONLY)
     endforeach()")

include_directories(${NSXLIB_SUBDIRS} ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Creating the library
# Under Windows and Miscrosoft Visual Studio the library will be static to avoid
# the DLL_EXPORT mess
if(MSVC OR NSX_BUILD_STATIC)
  add_library(nsx STATIC ${NSXLIB_SOURCES})
else()
  add_library(nsx SHARED ${NSXLIB_SOURCES})
endif()

add_custom_target(copy-nsx-headers COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/copy-nsxlib-headers.cmake)
add_dependencies(copy-nsx-headers generate-nsx-resources)
add_dependencies(nsx copy-nsx-headers generate-nsx-resources)

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
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/nsxlib/ DESTINATION include/nsxlib FILES_MATCHING PATTERN "*.h")

if(NSX_TIDY)
  add_tidy_target(nsx)
endif()
