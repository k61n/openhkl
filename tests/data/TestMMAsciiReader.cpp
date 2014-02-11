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
	MMILLAsciiReader* reader= new MMILLAsciiReader(std::string("D10_ascii_example"));
	MetaData* meta=reader->readMetaDataBlock();

	//
	Eigen::MatrixXi m;
	// Make sure that total counts for the first frame is 66.
	std::vector<int> v=reader->readBlock(0);
	Eigen::Map<Eigen::MatrixXi> map(&(v[0]),32,32);
	//
	BOOST_CHECK(map.sum()==65);

}
