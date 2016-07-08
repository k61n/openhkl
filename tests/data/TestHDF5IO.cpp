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
#include "DiffractometerStore.h"
#include "ILLAsciiData.h"
#include "HDF5Data.h"
#include "Units.h"

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_HDF5_IO)
{
	DiffractometerStore* ds = DiffractometerStore::Instance();

	std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D10"));

	ILLAsciiData dataf("D10_ascii_example",diff);

	dataf.open();
    dataf.readInMemory(nullptr);

	dataf.saveHDF5("D10_hdf5_example.h5");
	dataf.close();
}
