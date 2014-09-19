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



