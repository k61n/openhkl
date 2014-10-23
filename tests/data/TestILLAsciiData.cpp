#define BOOST_TEST_MODULE "Test ILL Ascii Data"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include "DiffractometerFactory.h"
#include "ILLAsciiData.h"

#include <boost/test/unit_test.hpp>

using namespace SX::Data;
using namespace SX::Instrument;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_ILL_Data)
{

	DiffractometerFactory* factory = DiffractometerFactory::Instance();

	std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(new Diffractometer(*(factory->create("D10-4c","D10 diffractometer"))));

	ILLAsciiData reader(std::string("D10_ascii_example"),diff,false);

	MetaData* meta=reader.metadata();

	BOOST_CHECK(meta->getKey<int>("nbang")==2);

	std::vector<int> v=reader.getFrame(0);

	// Map the vector to a matrix (no copying)
	Eigen::Map<Eigen::MatrixXi> map(&(v[0]),32,32);

	// Check the total number of count in the frame 0
	BOOST_CHECK_EQUAL(map.sum(),65);

	// Check the value of the monitor
	BOOST_CHECK_CLOSE(meta->getKey<double>("monitor"),20000,tolerance);

}
