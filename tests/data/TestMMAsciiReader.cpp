#define BOOST_TEST_MODULE "Test Ascii format"
#define BOOST_TEST_DYN_LINK
#include "MMILLAsciiReader.h"
#include "MetaData.h"
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

using namespace SX::Data;

BOOST_AUTO_TEST_CASE(Test_Ascii_Reader)
{
	MMILLAsciiReader* reader= new MMILLAsciiReader();
	reader->mapFile(std::string("D10_ascii_example"));
	//
	Eigen::MatrixXi m;
	// Make sure that total counts for the first frame is 65.
	std::vector<int> v=reader->readBlock(0);
	// Map the vector to a matrix (no copying)
	Eigen::Map<Eigen::MatrixXi> map(&(v[0]),32,32);
	//
	BOOST_CHECK(map.sum()==65);

}
