#define BOOST_TEST_MODULE "Test New Work Flow"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/utils/Units.h>

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

// const double tolerance=1e-2;

int run_test();

BOOST_AUTO_TEST_CASE(Test_NewWorkFlow)
{
    BOOST_CHECK_EQUAL(run_test(), 0);
}

int run_test()
{
    DataReaderFactory* factory = DataReaderFactory::Instance();
    DiffractometerStore* ds = DiffractometerStore::Instance();

    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("BioDiff2500"));
    std::shared_ptr<DataSet> dataf(factory->create("hdf", "H5_example.hdf", diff));

    // MetaData* meta=dataf->getMetadata();

    //BOOST_CHECK(meta->getKey<int>("nbang")==2);

    dataf->open();
    Eigen::MatrixXi v=dataf->getFrame(0);

    std::cout << "the sum is " << v.sum() << std::endl;

    // Check the total number of count in the frame 0
    BOOST_CHECK_EQUAL(v.sum(), 1282584565);

    dataf->close();
    return 0;
}
