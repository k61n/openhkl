# unit test for RotAxis class
add_executable(TestRotAxis TestRotAxis.cpp)
TARGET_LINK_LIBRARIES(TestRotAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestRotAxis TestRotAxis)

configure_file(RotAxis.xml RotAxis.xml COPYONLY)

# unit test for DetectorMapping Interface
#add_executable(TestDetectorMapping TestDetectorMapping.cpp)
#TARGET_LINK_LIBRARIES(TestDetectorMapping ${Boost_LIBRARIES} NSXTool)
#add_test(TestDetectorMapping TestDetectorMapping)
