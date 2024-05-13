project(openhkltests)

file(GLOB_RECURSE unit_test_sources "unit/*/*.cpp")
file(GLOB_RECURSE func_test_sources "functional/*.cpp")
file(GLOB_RECURSE nightly_test_sources "nightly/*.cpp")

add_library(catch2 catch2.cpp)
target_include_directories(catch2 PRIVATE ${CMAKE_SOURCE_DIR})

foreach(source_file ${unit_test_sources})
  get_filename_component(base_name ${source_file} NAME_WE)
  add_executable(${base_name} ${source_file})
  target_link_options(${base_name} PRIVATE "LINKER:-as-needed")
  target_include_directories(${base_name} PRIVATE ${CMAKE_SOURCE_DIR})
  target_link_libraries(${base_name} catch2 openhklcore)

  if(${OHKL_VALGRIND})
    add_test(NAME ${base_name}
        COMMAND valgrind --error-exitcode=3 --leak-check=full --suppressions=${CMAKE_CURRENT_BINARY_DIR}/suppressions --gen-suppressions=all $<TARGET_FILE:${base_name}>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data)
    set_tests_properties(${base_name} PROPERTIES LABELS "unit")
  else()
    add_test(NAME ${base_name} COMMAND ${base_name}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data)
    set_tests_properties(${base_name} PROPERTIES LABELS "unit")
  endif()
endforeach(source_file)

foreach(source_file ${func_test_sources})
  get_filename_component(base_name ${source_file} NAME_WE)
  add_executable(${base_name} ${source_file})
  target_link_options(${base_name} PRIVATE "LINKER:-as-needed")
  target_include_directories(${base_name} PRIVATE ${CMAKE_SOURCE_DIR})
  target_link_libraries(${base_name} catch2 openhklcore)

  if(${OHKL_VALGRIND})
    add_test(NAME ${base_name}
      COMMAND valgrind --error-exitcode=3 --leak-check=full --suppressions=${CMAKE_CURRENT_BINARY_DIR}/suppressions --gen-suppressions=all $<TARGET_FILE:${base_name}>
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data)
    set_tests_properties(${base_name} PROPERTIES LABELS "functional")
  else()
    add_test(NAME ${base_name} COMMAND ${base_name}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data)
    set_tests_properties(${base_name} PROPERTIES LABELS "functional")
  endif()
endforeach(source_file)

if (OHKL_FULL_WORKFLOW_TEST)
  foreach(source_file ${nightly_test_sources})
    get_filename_component(base_name ${source_file} NAME_WE)
    add_executable(${base_name} ${source_file})
  target_link_options(${base_name} PRIVATE "LINKER:-as-needed")
    target_include_directories(${base_name} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${base_name} catch2 openhklcore)

    if(${OHKL_VALGRIND})
      add_test(NAME ${base_name}
        COMMAND valgrind --error-exitcode=3 --leak-check=full --suppressions=${CMAKE_CURRENT_BINARY_DIR}/suppressions --gen-suppressions=all $<TARGET_FILE:${base_name}>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data)
      set_tests_properties(${base_name} PROPERTIES LABELS "nightly")
    else()
      add_test(NAME ${base_name} COMMAND ${base_name}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/data)
      set_tests_properties(${base_name} PROPERTIES LABELS "nightly")
    endif()
  endforeach(source_file)
endif()

# valgrind suppressions
configure_file(valgrind/suppressions suppressions COPYONLY)
