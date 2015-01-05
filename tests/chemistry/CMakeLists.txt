add_executable(TestIsotopeManager TestIsotopeManager.cpp)
TARGET_LINK_LIBRARIES(TestIsotopeManager ${Boost_LIBRARIES} NSXTool)
add_test(TestIsotopeManager TestIsotopeManager)

configure_file(../../resources/isotopes/SXIsotopeDatabase.xml SXIsotopeDatabase.xml COPYONLY)
