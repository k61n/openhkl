add_executable(TestMetaData TestMetaData.cpp)
TARGET_LINK_LIBRARIES(TestMetaData ${Boost_LIBRARIES} NSXTool)
add_test(TestMetaData TestMetaData)

add_executable(TestHDF5IO TestHDF5IO.cpp)
TARGET_LINK_LIBRARIES(TestHDF5IO ${Boost_LIBRARIES} NSXTool)
add_test(TestHDF5IO TestHDF5IO)

add_executable(TestILLAsciiData TestILLAsciiData.cpp)
TARGET_LINK_LIBRARIES(TestILLAsciiData ${Boost_LIBRARIES} NSXTool)
add_test(TestILLAsciiData TestILLAsciiData)

add_executable(TestILLAsciiQScan TestILLAsciiQScan.cpp)
TARGET_LINK_LIBRARIES(TestILLAsciiQScan ${Boost_LIBRARIES} NSXTool)
add_test(TestILLAsciiQScan TestILLAsciiQScan)

add_executable(TestPeakInData TestPeakInData.cpp)
TARGET_LINK_LIBRARIES(TestPeakInData ${Boost_LIBRARIES} NSXTool)
add_test(TestPeakInData TestPeakInData)

configure_file(D10_ascii_example D10_ascii_example COPYONLY)
configure_file(D9_ascii_example D9_ascii_example COPYONLY)
configure_file(D9_QSCAN D9_QSCAN COPYONLY)

