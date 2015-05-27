add_executable(TestUnitCell TestUnitCell.cpp)
TARGET_LINK_LIBRARIES(TestUnitCell ${Boost_LIBRARIES} NSXTool)
add_test(TestUnitCell TestUnitCell)

add_executable(TestLatticeFinder TestLatticeFinder.cpp)
TARGET_LINK_LIBRARIES(TestLatticeFinder ${Boost_LIBRARIES} NSXTool)
add_test(TestLatticeFinder TestLatticeFinder)

add_executable(TestNiggliReduction TestNiggliReduction.cpp)
TARGET_LINK_LIBRARIES(TestNiggliReduction ${Boost_LIBRARIES} NSXTool)
add_test(TestNiggliReduction TestNiggliReduction)

add_executable(TestFFTIndexing TestFFTIndexing.cpp)
TARGET_LINK_LIBRARIES(TestFFTIndexing ${Boost_LIBRARIES} NSXTool)
add_test(TestFFTIndexing TestFFTIndexing)

add_executable(TestPeak3D TestPeak3D.cpp)
TARGET_LINK_LIBRARIES(TestPeak3D ${Boost_LIBRARIES} NSXTool)
add_test(TestPeak3D TestPeak3D)

add_executable(TestUBMinimizer TestUBMinimizer.cpp)
TARGET_LINK_LIBRARIES(TestUBMinimizer ${Boost_LIBRARIES} NSXTool)
add_test(TestUBMinimizer TestUBMinimizer)

add_executable(TestSpaceGroup TestSpaceGroup.cpp)
TARGET_LINK_LIBRARIES(TestSpaceGroup ${Boost_LIBRARIES} NSXTool)
add_test(TestSpaceGroup TestSpaceGroup)

add_executable(TestSpaceGroupSymbols TestSpaceGroupSymbols.cpp)
TARGET_LINK_LIBRARIES(TestSpaceGroupSymbols ${Boost_LIBRARIES} NSXTool)
add_test(TestSpaceGroupSymbols TestSpaceGroupSymbols)

add_executable(TestSymOp TestSymOp.cpp)
TARGET_LINK_LIBRARIES(TestSymOp ${Boost_LIBRARIES} NSXTool)
add_test(TestSymOp TestSymOp)

configure_file(CsOsO_15K.raf CsOsO_15K.raf COPYONLY)
configure_file(CsOsO_15K.lpt CsOsO_15K.lpt COPYONLY)
