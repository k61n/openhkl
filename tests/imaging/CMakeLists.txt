# unit tests for kernel classes

add_executable(TestConstantKernel TestConstantKernel.cpp)
add_executable(TestAnnularKernel TestAnnularKernel.cpp)

target_link_libraries(TestConstantKernel ${Boost_LIBRARIES} NSXTool)
target_link_libraries(TestAnnularKernel ${Boost_LIBRARIES} NSXTool)

add_test(TestConstantKernel TestConstantKernel)
add_test(TestAnnularKernel TestAnnularKernel)
