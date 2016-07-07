# unit tests for kernel classes

add_executable(TestConstantKernel TestConstantKernel.cpp)
add_executable(TestAnnularKernel TestAnnularKernel.cpp)
add_executable(TestDeltaKernel TestDeltaKernel.cpp)
add_executable(TestConvolver TestConvolver.cpp)

target_link_libraries(TestConstantKernel ${Boost_LIBRARIES} NSXTool)
target_link_libraries(TestAnnularKernel ${Boost_LIBRARIES} NSXTool)
target_link_libraries(TestDeltaKernel ${Boost_LIBRARIES} NSXTool)
target_link_libraries(TestConvolver ${Boost_LIBRARIES} NSXTool)

add_test(TestConstantKernel TestConstantKernel)
add_test(TestAnnularKernel TestAnnularKernel)
add_test(TestDeltaKernel TestDeltaKernel)
add_test(TestConvolver TestConvolver)

