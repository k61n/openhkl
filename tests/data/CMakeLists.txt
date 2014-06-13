add_executable(TestMetaData TestMetaData.cpp)
TARGET_LINK_LIBRARIES(TestMetaData ${Boost_LIBRARIES} NSXTool)
add_test(TestMetaData TestMetaData)

add_executable(TestILLAsciiMetaReader TestILLAsciiMetaReader.cpp)
TARGET_LINK_LIBRARIES(TestILLAsciiMetaReader ${Boost_LIBRARIES} NSXTool)
add_test(TestILLAsciiMetaReader TestILLAsciiMetaReader)

add_executable(TestMMAsciiReader TestMMAsciiReader.cpp)
TARGET_LINK_LIBRARIES(TestMMAsciiReader ${Boost_LIBRARIES} NSXTool)
add_test(TestMMAsciiReader TestMMAsciiReader)

configure_file(D10_ascii_example D10_ascii_example COPYONLY)


