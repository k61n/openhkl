#define BOOST_TEST_MODULE "Test ILL Ascii Data"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "ComponentState.h"
#include "DiffractometerFactory.h"
#include "ILLAsciiData.h"
#include "Units.h"

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_ILL_Data)
{

	DiffractometerFactory* factory = DiffractometerFactory::Instance();

	std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(new Diffractometer(*(factory->create("D10-4c","D10 diffractometer"))));

	ILLAsciiData reader(std::string("D10_ascii_example"),diff,false);

	MetaData* meta=reader.getMetadata();

	BOOST_CHECK(meta->getKey<int>("nbang")==2);

	std::vector<int> v=reader.getFrame(0);

	// Map the vector to a matrix (no copying)
	Eigen::Map<Eigen::MatrixXi> map(&(v[0]),32,32);

	// Check the total number of count in the frame 0
	BOOST_CHECK_EQUAL(map.sum(),65);

	// Check the value of the monitor
	BOOST_CHECK_CLOSE(meta->getKey<double>("monitor"),20000,tolerance);

	const std::vector<ComponentState> detectorStates=reader.getDetectorStates();
	const std::vector<ComponentState> sampleStates=reader.getSampleStates();

	BOOST_CHECK_CLOSE(detectorStates[3].getValues()[0],0.54347000E+05*deg/1000.0,tolerance);
	BOOST_CHECK_CLOSE(sampleStates[2].getValues()[0],0.26572000E+05*deg/1000.0,tolerance);
	BOOST_CHECK_CLOSE(sampleStates[2].getValues()[1],0.48923233E+02*deg,tolerance);
	BOOST_CHECK_CLOSE(sampleStates[2].getValues()[2],-0.48583171E+02*deg,tolerance);

	ComponentState cs=reader.getDetectorInterpolatedState(0.0);
	BOOST_CHECK_CLOSE(cs.getValues()[0],detectorStates[0].getValues()[0],tolerance);

	cs=reader.getDetectorInterpolatedState(0.5);
	BOOST_CHECK_CLOSE(cs.getValues()[0],detectorStates[0].getValues()[0]+0.5*(detectorStates[1].getValues()[0]-detectorStates[0].getValues()[0]),tolerance);

	cs=reader.getDetectorInterpolatedState(2.3);
	BOOST_CHECK_CLOSE(cs.getValues()[0],detectorStates[2].getValues()[0]+0.3*(detectorStates[3].getValues()[0]-detectorStates[2].getValues()[0]),tolerance);

}
