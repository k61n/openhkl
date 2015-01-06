add_executable(TestIsotope TestIsotope.cpp)
TARGET_LINK_LIBRARIES(TestIsotope ${Boost_LIBRARIES} NSXTool)
add_test(TestIsotope TestIsotope)

add_executable(TestElement TestElement.cpp)
TARGET_LINK_LIBRARIES(TestElement ${Boost_LIBRARIES} NSXTool)
add_test(TestElement TestElement)

configure_file(../../resources/isotopes/isotopes.xml isotopes.xml COPYONLY)
configure_file(../../resources/isotopes/elements.xml elements.xml COPYONLY)
