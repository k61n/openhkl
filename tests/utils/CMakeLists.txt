# unit test for Path class
add_executable(TestPath TestPath.cpp)
TARGET_LINK_LIBRARIES(TestPath ${Boost_LIBRARIES} NSXTool)
add_test(TestPath TestPath)
