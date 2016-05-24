# unit test for Box Kernel class
add_executable(TestBoxKernel TestBoxKernel.cpp)
TARGET_LINK_LIBRARIES(TestBoxKernel ${Boost_LIBRARIES} NSXTool)
add_test(TestBoxKernel TestBoxKernel)
