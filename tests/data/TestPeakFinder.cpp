#include <vector>

#include <Eigen/Dense>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/ProgressHandler.h>

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

    peakFinder.setMaxComponents(10000);
    NSX_CHECK_ASSERT(peakFinder.getMaxComponents() == 10000);

    peakFinder.setMinComponents(10);
    NSX_CHECK_ASSERT(peakFinder.getMinComponents() == 10);

    peakFinder.setThresholdValue(3.0);
    NSX_CHECK_CLOSE(peakFinder.getThresholdValue(), 3.0, 1e-10);

    auto found_peaks = peakFinder.find(numors);
    size_t num_peaks = found_peaks.size();

    NSX_CHECK_ASSERT(num_peaks == 1);

    dataf->close();
}
