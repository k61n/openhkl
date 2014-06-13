#define BOOST_TEST_MODULE "Test Ascii format"
#define BOOST_TEST_DYN_LINK
#include "MetaData.h"
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>
#include "DataReaderFactory.h"
using namespace SX::Data;

BOOST_AUTO_TEST_CASE(Test_Ascii_Reader)
{
	DataReaderFactory* readers=DataReaderFactory::Instance();
	IDataReader* reader=readers->create("ILL-Ascii");
	reader->open(std::string("D10_ascii_example"));

	// Make sure that total counts for the first frame is 65.
	MetaData* meta=reader->getMetaData();

	BOOST_CHECK(meta->getKey<int>("nbang")==2);

	std::vector<int> v=reader->getFrame(0);
	// Map the vector to a matrix (no copying)
	Eigen::Map<Eigen::MatrixXi> map(&(v[0]),32,32);
	//
	BOOST_CHECK(map.sum()==65);

	delete meta;

}
