add_executable(TestUnitCell TestUnitCell.cpp)
TARGET_LINK_LIBRARIES(TestUnitCell ${Boost_LIBRARIES} NSXTool)
add_test(TestUnitCell TestUnitCell)



