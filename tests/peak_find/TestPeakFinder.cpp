#include <vector>

#include <Eigen/Dense>

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

    nsx::sptrDataSet dataset(new nsx::DataSet("", "D10_ascii_example", experiment.diffractometer()));
    const auto& metadata = dataset->reader()->metadata();

    NSX_CHECK_ASSERT(metadata.key<int>("nbang")==2);

    nsx::DataList datasets;
    datasets.push_back(dataset);

    nsx::PeakFinder peakFinder(datasets);

    peakFinder.setPeakMergingScale(1.0);

    peakFinder.setMaxSize(10000);
    NSX_CHECK_ASSERT(peakFinder.maxSize() == 10000);

    peakFinder.setMinSize(10);
    NSX_CHECK_ASSERT(peakFinder.minSize() == 10);

    peakFinder.setMaxFrames(10);
    NSX_CHECK_ASSERT(peakFinder.maxFrames() == 10);

    peakFinder.run();

    auto found_peaks = peakFinder.peaks();

    size_t num_peaks = found_peaks.size();

    NSX_CHECK_ASSERT(num_peaks == 1);

    dataset->close();
}
