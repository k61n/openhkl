#define BOOST_TEST_MODULE "Test XDS output class"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/data/XDS.h>
#include <nsxlib/data/IData.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/instrument/DiffractometerStore.h>

using IData = SX::Data::DataSet;
using DataReaderFactory = SX::Data::DataReaderFactory;
using DiffractometerStore = SX::Instrument::DiffractometerStore;
using Diffractometer = SX::Instrument::Diffractometer;

// const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_XDS)
{
#pragma warning "test not implemented"
    DataReaderFactory* factory = DataReaderFactory::Instance();
    DiffractometerStore* ds = DiffractometerStore::Instance();

    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("BioDiff2500"));
    std::shared_ptr<IData> dataf(factory->create("hdf", "H5_example.hdf", diff));

    // MetaData* meta=dataf->getMetadata();

    //BOOST_CHECK(meta->getKey<int>("nbang")==2);

    dataf->open();
    Eigen::MatrixXi v=dataf->getFrame(0);

    volatile size_t nframes = dataf->getNFrames();

    std::cout << "the number of frames is " << nframes << std::endl;

    std::cout << "the sum is " << v.sum() << std::endl;

    // Check the total number of count in the frame 0
    BOOST_CHECK_EQUAL(v.sum(), 1282584565);

    dataf->close();
}
