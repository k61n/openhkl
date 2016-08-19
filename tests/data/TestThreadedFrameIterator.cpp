#define BOOST_TEST_MODULE "Test threaded frame iterator"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "ThreadedFrameIterator.h"
#include "DiffractometerStore.h"
#include "Diffractometer.h"
#include "ILLAsciiData.h"

using namespace SX::Data;
using namespace SX::Instrument;

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_ThreadedFrameIterator)
{
    DiffractometerStore* ds = DiffractometerStore::Instance();
    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D10"));
    ILLAsciiData dataf(std::string("D10_ascii_example"), diff);

    dataf.setIteratorCallback([](IData* data, int idx) {return new ThreadedFrameIterator(data, idx);});

    std::unique_ptr<IFrameIterator> it(dataf.getIterator(0));

    for (; it->index() != dataf.getNFrames(); it->advance()) {
        auto frame = it->getFrame();
    }
}
