#define BOOST_TEST_MODULE "Test HDF5 writting and reading using Blosc library"
#define BOOST_TEST_DYN_LINK

#include <stdexcept>
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
    DiffractometerStore* ds;
    std::shared_ptr<Diffractometer> diff;
    std::unique_ptr<ILLAsciiData> dataf;

    std::cout << "beginning Test_HDF5_IO" << std::endl;

    try {
        ds = DiffractometerStore::Instance();
        diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D10"));
        dataf = std::unique_ptr<ILLAsciiData>(new ILLAsciiData("D10_ascii_example",diff));
        dataf->open();
        dataf->readInMemory(nullptr);

        std::cout << "ascii data successfully read. writing hdf5..." << std::endl;

        dataf->saveHDF5("D10_hdf5_example.h5");
    }
    catch (std::exception& e) {
        BOOST_FAIL(std::string("saveHDF5() threw exception: ") + e.what());
    }
    catch(...) {
        BOOST_FAIL("saveHDF5() threw unknown exception");
    }

    dataf->close();
}
