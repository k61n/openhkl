#include "test/cpp/catch.hpp"

#include <vector>
#include <Eigen/Dense>

#include "core/data/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Experiment.h"
#include "core/data/IDataReader.h"
#include "core/data/MetaData.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/utils/ProgressHandler.h"

TEST_CASE("test/peak_find/TestPeakFinder.cpp", "") {

    nsx::Experiment experiment("exp", "D10");

    nsx::DataList numors;

    nsx::DataReaderFactory factory;
    auto dataf = factory.create("", "D10_ascii_example", experiment.diffractometer());
    const auto& metadata = dataf->reader()->metadata();
    nsx::PeakFinder peakFinder;
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);

    CHECK(metadata.key<int>("nbang") == 2);

    dataf->open();
    numors.push_back(dataf);
    peakFinder.setHandler(handler);

    peakFinder.setPeakScale(1.0);

    auto found_peaks = peakFinder.find(numors);
    size_t num_peaks = found_peaks.size();

    CHECK(num_peaks == 1);

    dataf->close();
}
