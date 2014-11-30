add_executable(TestMetaData TestMetaData.cpp)
TARGET_LINK_LIBRARIES(TestMetaData ${Boost_LIBRARIES} NSXTool)
add_test(TestMetaData TestMetaData)

add_executable(TestILLAsciiMetaReader TestILLAsciiMetaReader.cpp)
TARGET_LINK_LIBRARIES(TestILLAsciiMetaReader ${Boost_LIBRARIES} NSXTool)
add_test(TestILLAsciiMetaReader TestILLAsciiMetaReader)

add_executable(TestMMAsciiReader TestMMAsciiReader.cpp)
TARGET_LINK_LIBRARIES(TestMMAsciiReader ${Boost_LIBRARIES} NSXTool)
add_test(TestMMAsciiReader TestMMAsciiReader)


add_executable(TestHDF5IO TestHDF5IO.cpp)
TARGET_LINK_LIBRARIES(TestHDF5IO ${Boost_LIBRARIES} NSXTool)
add_test(TestHDF5IO TestHDF5IO)

add_executable(TestILLAsciiData TestILLAsciiData.cpp)
TARGET_LINK_LIBRARIES(TestILLAsciiData ${Boost_LIBRARIES} NSXTool)
add_test(TestILLAsciiData TestILLAsciiData)

configure_file(D10_ascii_example D10_ascii_example COPYONLY)


