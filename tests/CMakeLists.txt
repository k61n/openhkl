add_executable(TestRotAxis TestRotAxis.cpp)
TARGET_LINK_LIBRARIES(TestRotAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestRotAxis TestRotAxis)

add_executable(TestILLAsciiMetaReader TestILLAsciiMetaReader.cpp)
TARGET_LINK_LIBRARIES(TestILLAsciiMetaReader ${Boost_LIBRARIES} NSXTool)
add_test(TestILLAsciiMetaReader TestILLAsciiMetaReader)

configure_file(D10_ascii_example D10_ascii_example COPYONLY)


