file(GLOB test_sources "Test*.cpp")

#add_library(catch2 catch2.cpp)
#target_include_directories(catch2 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

foreach(source_file ${test_sources})
    get_filename_component(base_name ${source_file} NAME_WE)
    add_executable(${base_name} ${source_file})
    target_include_directories(${base_name}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/3rdparty/xsection/lib)
    target_link_libraries(${base_name} ${Boost_LIBRARIES} xsection catch2)
    add_test(NAME ${base_name} COMMAND ${base_name})
endforeach(source_file)

add_custom_target(check.XSection COMMAND ${CMAKE_CTEST_COMMAND})
