add_executable(TestIsotope TestIsotope.cpp)
TARGET_LINK_LIBRARIES(TestIsotope ${Boost_LIBRARIES} NSXTool)
add_test(TestIsotope TestIsotope)

add_executable(TestElement TestElement.cpp)
TARGET_LINK_LIBRARIES(TestElement ${Boost_LIBRARIES} NSXTool)
add_test(TestElement TestElement)

add_executable(TestMaterial TestMaterial.cpp)
TARGET_LINK_LIBRARIES(TestMaterial ${Boost_LIBRARIES} NSXTool)
add_test(TestMaterial TestMaterial)

configure_file(../../resources/databases/isotopes.xml isotopes.xml COPYONLY)
configure_file(../../resources/databases/elements.xml elements.xml COPYONLY)
configure_file(../../resources/databases/materials.xml materials.xml COPYONLY)
