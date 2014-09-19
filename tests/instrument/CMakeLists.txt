# unit test for Axis class
add_executable(TestAxis TestAxis.cpp)
TARGET_LINK_LIBRARIES(TestAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestAxis TestAxis)

# unit test for RotAxis class
add_executable(TestRotAxis TestRotAxis.cpp)
TARGET_LINK_LIBRARIES(TestRotAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestRotAxis TestRotAxis)

configure_file(RotAxis.xml RotAxis.xml COPYONLY)

# unit test for TransAxis class
add_executable(TestTransAxis TestTransAxis.cpp)
TARGET_LINK_LIBRARIES(TestTransAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestTransAxis TestTransAxis)

# unit test for Gonio class
add_executable(TestGonio TestGonio.cpp)
TARGET_LINK_LIBRARIES(TestGonio ${Boost_LIBRARIES} NSXTool)
add_test(TestGonio TestGonio)

# unit test for Cylindrical detector
add_executable(TestCylindricalDetector TestCylindricalDetector.cpp)
TARGET_LINK_LIBRARIES(TestCylindricalDetector ${Boost_LIBRARIES} NSXTool)
add_test(TestCylindricalDetector TestCylindricalDetector)

# unit test for Flat detector
add_executable(TestFlatDetector TestFlatDetector.cpp)
TARGET_LINK_LIBRARIES(TestFlatDetector ${Boost_LIBRARIES} NSXTool)
add_test(TestFlatDetector TestFlatDetector)

# unit test for Sample Component
add_executable(TestSample TestSample.cpp)
TARGET_LINK_LIBRARIES(TestSample ${Boost_LIBRARIES} NSXTool)
add_test(TestSample TestSample)

