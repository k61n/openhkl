#define BOOST_TEST_MODULE "Test HDF5 writting and reading using Blosc library"
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

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_HDF5_IO)
{
	DiffractometerFactory* factory = DiffractometerFactory::Instance();

	std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(factory->create("D10 4-circles","D10 diffractometer")->clone());

	ILLAsciiData reader(std::string("D10_ascii_example"),diff,false);

	reader.map();
	reader.loadAllFrames();
	const std::vector<Eigen::MatrixXi>& data=reader.getData();

	// Store sum of counts in each frame
	std::vector<int> sumsbefore(data.size());
	for (std::size_t i=0;i<data.size();++i)
	{
		sumsbefore[i]=data[i].sum();
	}

	reader.saveHDF5("114047.h5");
	// Read back the data
	reader.readHDF5("114047.h5");

	const std::vector<Eigen::MatrixXi>& data2=reader.getData();

	assert(data.size()==data2.size());

	std::vector<int> sumsafter(data2.size());
	for (std::size_t i=0;i<data2.size();++i)
	{
		sumsafter[i]=data[i].sum();
		assert(sumsafter[i]==sumsbefore[i]);
	}

}
