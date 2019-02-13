###########################################################################
# Setup resources manager helper program
###########################################################################

add_executable(make_single_resource ${CMAKE_SOURCE_DIR}/cmake/scripts/make_single_resource.cpp)
add_executable(update_resources ${CMAKE_SOURCE_DIR}/cmake/scripts/update_resources.cpp)

configure_file(${CMAKE_SOURCE_DIR}/core/resources/Resources.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/Resources.h)
configure_file(${CMAKE_SOURCE_DIR}/core/resources/Resources.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/tmp/Resources.cpp)

set(core_RESOURCES_CPP ${CMAKE_CURRENT_BINARY_DIR}/generated/Resources.cpp)

file(GLOB_RECURSE NSX_RESOURCES ${CMAKE_SOURCE_DIR}/resources/*.yml)
foreach (NSX_RESOURCE ${NSX_RESOURCES})
    get_filename_component(NSX_RESOURCE_NAME ${NSX_RESOURCE} NAME_WE)
    get_filename_component(NSX_RESOURCE_DIR ${NSX_RESOURCE} DIRECTORY)
    get_filename_component(NSX_RESOURCE_TYPE ${NSX_RESOURCE_DIR} NAME)

    configure_file(${CMAKE_SOURCE_DIR}/core/resources/SingleResource.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/SingleResource${NSX_RESOURCE_NAME}.h)
    configure_file(${CMAKE_SOURCE_DIR}/core/resources/SingleResource.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/tmp/SingleResource${NSX_RESOURCE_NAME}.cpp)
    list(APPEND core_RESOURCES_CPP ${CMAKE_CURRENT_BINARY_DIR}/generated/SingleResource${NSX_RESOURCE_NAME}.cpp)
    add_custom_command(
        OUTPUT  ${CMAKE_BINARY_DIR}/core/generated/SingleResource${NSX_RESOURCE_NAME}.cpp
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/core/tmp/SingleResource${NSX_RESOURCE_NAME}.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated/SingleResource${NSX_RESOURCE_NAME}.cpp
        COMMAND ${CMAKE_BINARY_DIR}/core/make_single_resource ${NSX_RESOURCE} ${CMAKE_CURRENT_BINARY_DIR}/generated/SingleResource${NSX_RESOURCE_NAME}.cpp
        DEPENDS ${CMAKE_BINARY_DIR}/core/make_single_resource ${CMAKE_SOURCE_DIR}/core/resources/SingleResource.h.in
        COMMENT "Generating ${CMAKE_BINARY_DIR}/core/generated/SingleResource${NSX_RESOURCE_NAME}.cpp resource"
    )
endforeach()

file(WRITE ${CMAKE_BINARY_DIR}/core/generate-nsx-resources.cmake 
 "execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tmp/Resources.cpp ${CMAKE_CURRENT_BINARY_DIR}/generated/Resources.cpp)
  foreach (NSX_RESOURCE ${NSX_RESOURCES})
    get_filename_component(NSX_RESOURCE_NAME \${NSX_RESOURCE} NAME_WE)
    get_filename_component(NSX_RESOURCE_DIR \${NSX_RESOURCE} DIRECTORY)
    get_filename_component(NSX_RESOURCE_TYPE \${NSX_RESOURCE_DIR} NAME)

    execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/update_resources \${NSX_RESOURCE_TYPE} \${NSX_RESOURCE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/generated/Resources.cpp)
  endforeach ()"
)

add_custom_target(generate-nsx-resources COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/generate-nsx-resources.cmake)
add_dependencies(generate-nsx-resources update_resources)

set_source_files_properties(${core_RESOURCES_CPP} PROPERTIES GENERATED TRUE)

# Set up the Version file
configure_file(${CMAKE_SOURCE_DIR}/core/Version.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/Version.h @ONLY)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/generated/Version.h PROPERTIES GENERATED TRUE)

file(GLOB core_GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/generated/*.h)

