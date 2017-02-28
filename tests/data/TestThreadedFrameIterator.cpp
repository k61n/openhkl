#define BOOST_TEST_MODULE "Test threaded frame iterator"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/data/ThreadedFrameIterator.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/data/DataReaderFactory.h>

using namespace SX::Data;
using namespace SX::Instrument;

// const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_ThreadedFrameIterator)
{
    auto factory = DataReaderFactory::Instance();
    DiffractometerStore* ds = DiffractometerStore::Instance();
    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D10"));
    std::shared_ptr<IData> dataf(factory->create("", "D10_ascii_example", diff));
    dataf->setIteratorCallback([](IData& data, int idx) {return new ThreadedFrameIterator(data, idx);});
    std::unique_ptr<IFrameIterator> it(dataf->getIterator(0));

    for (; it->index() != dataf->getNFrames(); it->advance()) {
        auto frame = it->getFrame();
    }
}
