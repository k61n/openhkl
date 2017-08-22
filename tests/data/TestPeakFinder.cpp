#define BOOST_TEST_MODULE "Test PeakFinder"
#define BOOST_TEST_DYN_LINK

#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/data/MetaData.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/utils/ProgressHandler.h>

BOOST_AUTO_TEST_CASE(Test_PeakFinder)
{
    nsx::DataList numors;

    nsx::DataReaderFactory factory;
    auto diff = nsx::Diffractometer::build("D10");
    auto dataf = factory.create("", "D10_ascii_example", diff);
    auto meta = dataf->getMetadata();
    nsx::PeakFinder peakFinder;
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);

    BOOST_CHECK(meta->getKey<int>("nbang")==2);

    dataf->open();
    numors.push_back(dataf);
    peakFinder.setHandler(handler);

    peakFinder.setConfidence(0.997);
    BOOST_CHECK_CLOSE(peakFinder.confidence(), 0.997, 1e-10);

    peakFinder.setMaxComponents(10000);
    BOOST_CHECK(peakFinder.getMaxComponents() == 10000);

    peakFinder.setMinComponents(10);
    BOOST_CHECK(peakFinder.getMinComponents() == 10);

    peakFinder.setThresholdValue(3.0);
    BOOST_CHECK_CLOSE(peakFinder.getThresholdValue(), 3.0, 1e-10);

    bool result = peakFinder.find(numors);
    BOOST_CHECK(result == true);

    size_t num_peaks = dataf->getPeaks().size();

    BOOST_CHECK(num_peaks == 1);

    dataf->close();
    dataf->clearPeaks();
    dataf = nullptr;
}
