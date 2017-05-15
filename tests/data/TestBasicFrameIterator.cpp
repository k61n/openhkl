#define BOOST_TEST_MODULE "Test basic frame iterator"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/data/BasicFrameIterator.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/data/DataReaderFactory.h>

using namespace nsx::Data;
using nsx::Data::DataReaderFactory;
using namespace nsx::Instrument;

// const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_BasicFrameIterator)
{
    auto factory = DataReaderFactory::Instance();
    DiffractometerStore* ds = DiffractometerStore::Instance();
    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D10"));
    std::unique_ptr<DataSet> dataf(factory->create("", "D10_ascii_example", diff));
    dataf->setIteratorCallback([](DataSet& data, int idx) {return new BasicFrameIterator(data, idx);});
    std::unique_ptr<IFrameIterator> it(dataf->getIterator(0));

    for (; it->index() != dataf->getNFrames(); it->advance()) {
        auto frame = it->getFrame();
    }
}
