#include "test/cpp/catch.hpp"

#include "base/utils/ProgressHandler.h"
#include "core/algo/DataReaderFactory.h"
#include "core/analyse/PeakFinder.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/MetaData.h"

TEST_CASE("test/peak_find/TestPeakFinder.cpp", "")
{
    nsx::Experiment experiment("exp", "D10");
    nsx::DataList numors;

    nsx::DataReaderFactory factory;
    auto dataf = factory.create("", "D10_ascii_example", experiment.diffractometer());
    const auto& metadata = dataf->reader()->metadata();
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);

    CHECK(metadata.key<int>("nbang") == 2);

    dataf->open();
    numors.push_back(dataf);

    nsx::PeakFinder peakFinder;
    peakFinder.setHandler(handler);
    peakFinder.setPeakScale(1.0);

    peakFinder.find(numors);
    auto found_peaks = peakFinder.currentPeaks();
    CHECK(found_peaks.size() == 1);

    dataf->close();
}
