#define BOOST_TEST_MODULE "Test PeakFinder"
#define BOOST_TEST_DYN_LINK

#include <memory>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/utils/ProgressHandler.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_PeakFinder)
{
    std::vector<std::shared_ptr<DataSet>> numors;

    DataReaderFactory factory;
    DiffractometerStore* ds = DiffractometerStore::Instance();
    std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractometer("D10"));
    std::shared_ptr<DataSet> dataf(factory.create("", "D10_ascii_example", diff));
    MetaData* meta = dataf->getMetadata();
    PeakFinder peakFinder;
    std::shared_ptr<ProgressHandler> handler(new ProgressHandler);

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
