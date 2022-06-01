project(openhkltests)

file(GLOB_RECURSE test_sources "*/*.cpp")

add_library(catch2 catch2.cpp)
target_include_directories(catch2 PRIVATE ${CMAKE_SOURCE_DIR})

foreach(source_file ${test_sources})
    get_filename_component(base_name ${source_file} NAME_WE)
    add_executable(${base_name} ${source_file})
    target_include_directories(${base_name} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${base_name} catch2 openhklcore)

    if(${OHKL_VALGRIND})
        add_test(
            NAME ${base_name}
            COMMAND valgrind --error-exitcode=3 --leak-check=full --suppressions=${CMAKE_CURRENT_BINARY_DIR}/suppressions --gen-suppressions=all $<TARGET_FILE:${base_name}>
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data
            )
    else()
        add_test(
            NAME ${base_name}
            COMMAND ${base_name}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data
            )
    endif()

endforeach(source_file)

# valgrind suppressions
configure_file(valgrind/suppressions suppressions COPYONLY)
