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
#include "HDF5Data.h"
#include "Units.h"

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_HDF5_IO)
{
	DiffractometerFactory* factory = DiffractometerFactory::Instance();

	std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(factory->create("D19 4-circles","D10 diffractometer")->clone());

	for (int i=114054;i<114066;++i)
	{
	std::ostringstream os;
	os << i;
	ILLAsciiData dataf(std::string("/home/chapon/Data/D19/July2014/data/DKDP/"+os.str()),diff);

	dataf.open();
	dataf.readInMemory();
	const std::vector<Eigen::MatrixXi>& data=dataf.getData();


	dataf.saveHDF5("/home/chapon/Data/D19/July2014/data/DKDP/"+os.str()+".h5");
	dataf.close();
	}


}
