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

	ILLAsciiData dataf(std::string("D10_ascii_example"),diff);

	dataf.open();
	dataf.readInMemory();
	const std::vector<Eigen::MatrixXi>& data=dataf.getData();


	dataf.saveHDF5("D10_HDF5_example.h5");
	dataf.close();
}
