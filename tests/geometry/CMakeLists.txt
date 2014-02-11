# unit test for AABB class
add_executable(TestAABB TestAABB.cpp)
TARGET_LINK_LIBRARIES(TestAABB ${Boost_LIBRARIES} NSXTool)
add_test(TestAABB TestAABB)

# unit test for Blob2D class
add_executable(TestBlob2D TestBlob2D.cpp)
TARGET_LINK_LIBRARIES(TestBlob2D ${Boost_LIBRARIES} NSXTool)
add_test(TestBlob2D TestBlob2D)

# unit test for RotAxis class
add_executable(TestRotAxis TestRotAxis.cpp)
TARGET_LINK_LIBRARIES(TestRotAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestRotAxis TestRotAxis)

# unit test for NDTree class
add_executable(TestNDTree TestNDTree.cpp)
TARGET_LINK_LIBRARIES(TestNDTree ${Boost_LIBRARIES} NSXTool)
add_test(TestNDTree TestNDTree)


