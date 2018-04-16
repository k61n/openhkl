#include <vector>

#include <Eigen/Dense>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/ProgressHandler.h>

NSX_INIT_TEST

int main()
{
    nsx::DataList numors;

    nsx::DataReaderFactory factory;
    auto diff = nsx::Diffractometer::build("D10");
    auto dataf = factory.create("", "D10_ascii_example", diff);
    auto meta = dataf->metadata();
    nsx::PeakFinder peakFinder;
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);

    NSX_CHECK_ASSERT(meta->getKey<int>("nbang")==2);

    dataf->open();
    numors.push_back(dataf);
    peakFinder.setHandler(handler);

    peakFinder.setSearchConfidence(0.997);
    NSX_CHECK_CLOSE(peakFinder.searchConfidence(), 0.997, 1e-10);

    peakFinder.setIntegrationConfidence(0.997);
    NSX_CHECK_CLOSE(peakFinder.integrationConfidence(), 0.997, 1e-10);

    peakFinder.setMaxSize(10000);
    NSX_CHECK_ASSERT(peakFinder.maxSize() == 10000);

    peakFinder.setMinSize(10);
    NSX_CHECK_ASSERT(peakFinder.minSize() == 10);

    peakFinder.setMaxFrames(10);
    NSX_CHECK_ASSERT(peakFinder.maxFrames() == 10);

    auto found_peaks = peakFinder.find(numors);
    size_t num_peaks = found_peaks.size();

    NSX_CHECK_ASSERT(num_peaks == 1);

    dataf->close();
}
