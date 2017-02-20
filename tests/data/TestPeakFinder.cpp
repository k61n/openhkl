#define BOOST_TEST_MODULE "Test PeakFinder"
#define BOOST_TEST_DYN_LINK

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
#include <nsxlib/utils/Units.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/utils/ProgressHandler.h>

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;
using namespace SX::Utils;

// const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_PeakFinder)
{
    std::vector<std::shared_ptr<IData>> numors;

    DiffractometerStore* ds = DiffractometerStore::Instance();
    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D10"));
    std::shared_ptr<IData> dataf(new ILLAsciiData(std::string("D10_ascii_example"), diff));
    MetaData* meta=dataf->getMetadata();
    PeakFinder peakFinder;
    std::shared_ptr<ProgressHandler> handler(new ProgressHandler);

    BOOST_CHECK(meta->getKey<int>("nbang")==2);

    dataf->open();
    numors.push_back(dataf);
    peakFinder.setHandler(handler);

    peakFinder.setConfidence(0.997);
    BOOST_CHECK_CLOSE(peakFinder.getConfidence(), 0.997, 1e-10);

    peakFinder.setMaxComponents(10000);
    BOOST_CHECK(peakFinder.getMaxComponents() == 10000);

    peakFinder.setMinComponents(10);
    BOOST_CHECK(peakFinder.getMinComponents() == 10);

    peakFinder.setThresholdValue(3.0);
    BOOST_CHECK_CLOSE(peakFinder.getThresholdValue(), 3.0, 1e-10);

    bool result = peakFinder.find(numors);
    BOOST_CHECK(result == true);

    size_t num_peaks = dataf->getPeaks().size();

    std::cout << "the result is " << result << std::endl;
    std::cout << "the number of peaks is    " << num_peaks << std::endl;

    BOOST_CHECK(num_peaks == 1);

    dataf->close();
    dataf->clearPeaks();
    dataf = nullptr;
}
