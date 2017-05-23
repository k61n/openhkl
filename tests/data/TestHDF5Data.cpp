#define BOOST_TEST_MODULE "Test HDF5 data class"
#define BOOST_TEST_DYN_LINK

#include <memory>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/DiffractometerStore.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_HDF5Data)
{
    DataReaderFactory factory;
    DiffractometerStore* ds = DiffractometerStore::Instance();

    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractometer("BioDiff2500"));
    std::shared_ptr<DataSet> dataf(factory.create("hdf", "H5_example.hdf", diff));

    dataf->open();
    Eigen::MatrixXi v = dataf->getFrame(0);

    // Check the total number of count in the frame 0
    BOOST_CHECK_EQUAL(v.sum(), 1282584565);

    dataf->close();
}
