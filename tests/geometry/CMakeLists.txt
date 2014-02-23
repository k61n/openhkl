# unit test for AABB class
add_executable(TestAABB TestAABB.cpp)
TARGET_LINK_LIBRARIES(TestAABB ${Boost_LIBRARIES} NSXTool)
add_test(TestAABB TestAABB)

# unit test for Blob2D class
add_executable(TestBlob2D TestBlob2D.cpp)
TARGET_LINK_LIBRARIES(TestBlob2D ${Boost_LIBRARIES} NSXTool)
add_test(TestBlob2D TestBlob2D)

# unit test for Blob3D class
add_executable(TestBlob3D TestBlob3D.cpp)
TARGET_LINK_LIBRARIES(TestBlob3D ${Boost_LIBRARIES} NSXTool)
add_test(TestBlob3D TestBlob3D)

# unit test for Ellipsoid class (3D case)
add_executable(TestEllipsoid3D TestEllipsoid3D.cpp)
TARGET_LINK_LIBRARIES(TestEllipsoid3D ${Boost_LIBRARIES} NSXTool)
add_test(TestEllipsoid3D TestEllipsoid3D)

# unit test for Ellipsoid class (2D case)
add_executable(TestEllipsoid2D TestEllipsoid2D.cpp)
TARGET_LINK_LIBRARIES(TestEllipsoid2D ${Boost_LIBRARIES} NSXTool)
add_test(TestEllipsoid2D TestEllipsoid2D)

# unit test for RotAxis class
add_executable(TestRotAxis TestRotAxis.cpp)
TARGET_LINK_LIBRARIES(TestRotAxis ${Boost_LIBRARIES} NSXTool)
add_test(TestRotAxis TestRotAxis)

# unit test for NDTree class
add_executable(TestNDTree TestNDTree.cpp)
TARGET_LINK_LIBRARIES(TestNDTree ${Boost_LIBRARIES} NSXTool)
add_test(TestNDTree TestNDTree)

# unit test for OBB class
add_executable(TestOBB TestOBB.cpp)
TARGET_LINK_LIBRARIES(TestOBB ${Boost_LIBRARIES} NSXTool)
add_test(TestOBB TestOBB)

