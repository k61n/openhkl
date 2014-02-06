add_executable(TestRotAxis TestRotAxis.cpp)
TARGET_LINK_LIBRARIES(TestRotAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestRotAxis TestRotAxis)



