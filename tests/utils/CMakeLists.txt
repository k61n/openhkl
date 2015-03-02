# unit test for Path class
add_executable(TestPath TestPath.cpp)
TARGET_LINK_LIBRARIES(TestPath ${Boost_LIBRARIES} NSXTool)
add_test(TestPath TestPath)

add_executable(TestEigenMatrixParser TestEigenMatrixParser.cpp)
TARGET_LINK_LIBRARIES(TestEigenMatrixParser ${Boost_LIBRARIES} NSXTool)
add_test(TestEigenMatrixParser TestEigenMatrixParser)

add_executable(TestChemicalFormulaParser TestChemicalFormulaParser.cpp)
TARGET_LINK_LIBRARIES(TestChemicalFormulaParser ${Boost_LIBRARIES} NSXTool)
add_test(TestChemicalFormulaParser TestChemicalFormulaParser)
