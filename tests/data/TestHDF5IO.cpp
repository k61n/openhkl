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

#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/data/ILLAsciiData.h>
#include <nsxlib/data/HDF5Data.h>
#include <nsxlib/utils/Units.h>

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

// const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_HDF5_IO)
{
    DiffractometerStore* ds;
    std::shared_ptr<Diffractometer> diff;
    std::unique_ptr<IData> dataf;

    std::vector<Eigen::MatrixXi> frames;

    std::cout << "beginning Test_HDF5_IO" << std::endl;

    try {
        ds = DiffractometerStore::Instance();
        diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D10"));
        dataf = std::unique_ptr<IData>(new ILLAsciiData("D10_ascii_example", diff));
        dataf->open();

        for (size_t i = 0; i < dataf->getNFrames(); ++i)
            frames.push_back(dataf->getFrame(i));

        std::cout << "ascii data successfully read. writing hdf5..." << std::endl;

        dataf->saveHDF5("D10_hdf5_example.h5");
        dataf->close();
        dataf.reset();

        std::cout << "verifying integrity of hdf5 data..." << std::endl;

        // read data back in and check that it agrees!
        dataf = std::unique_ptr<IData>(new HDF5Data("D10_hdf5_example.h5", diff));

        for (size_t i = 0; i < dataf->getNFrames(); ++i) {
            BOOST_CHECK(dataf->getFrame(i) == frames[i]);
        }

        std::cout << "hdf5 data agrees with original ascii data" << std::endl;
    }
    catch (std::exception& e) {
        BOOST_FAIL(std::string("saveHDF5() threw exception: ") + e.what());
    }
    catch(...) {
        BOOST_FAIL("saveHDF5() threw unknown exception");
    }

    dataf->close();
}
