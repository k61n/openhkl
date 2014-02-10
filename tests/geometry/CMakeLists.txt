add_executable(TestAABB TestAABB.cpp)
TARGET_LINK_LIBRARIES(TestAABB ${Boost_LIBRARIES} NSXTool)
add_test(TestAABB TestAABB)

add_executable(TestBlob2D TestBlob2D.cpp)
TARGET_LINK_LIBRARIES(TestBlob2D ${Boost_LIBRARIES} NSXTool)
add_test(TestBlob2D TestBlob2D)


add_executable(TestRotAxis TestRotAxis.cpp)
TARGET_LINK_LIBRARIES(TestRotAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestRotAxis TestRotAxis)



