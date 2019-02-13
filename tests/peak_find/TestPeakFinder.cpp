#include <vector>

#include <Eigen/Dense>

#include <core/DataReaderFactory.h>
#include <core/Experiment.h>
#include <core/IDataReader.h>
#include <core/PeakFinder.h>
#include <core/DataSet.h>
#include <core/MetaData.h>
#include <core/Diffractometer.h>
#include <core/NSXTest.h>
#include <core/ProgressHandler.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("exp","D10");

    nsx::DataList numors;

    nsx::DataReaderFactory factory;
    auto dataf = factory.create("", "D10_ascii_example", experiment.diffractometer());
    const auto& metadata = dataf->reader()->metadata();
    nsx::PeakFinder peakFinder;
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);

    NSX_CHECK_ASSERT(metadata.key<int>("nbang")==2);

    dataf->open();
    numors.push_back(dataf);
    peakFinder.setHandler(handler);

    peakFinder.setPeakScale(1.0);

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
