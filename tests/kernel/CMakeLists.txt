# unit test for Factory class
add_executable(TestFactory TestFactory.cpp)
TARGET_LINK_LIBRARIES(TestFactory ${Boost_LIBRARIES} NSXTool)
add_test(TestFactory TestFactory)
