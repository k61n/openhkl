#include "test/cpp/catch.hpp"

#include "base/geometry/Ellipsoid.h"
#include "base/utils/ProgressHandler.h"
#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
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
    experiment.addData(dataf);

    dataf->open();
    numors.push_back(dataf);

    nsx::PeakFinder peakFinder;
    peakFinder.setHandler(handler);
    peakFinder.setPeakScale(1.0);
    peakFinder.setThreshold(10);
    peakFinder.setMinSize(10);
    peakFinder.setMaxSize(10000);
    peakFinder.setFramesBegin(0);
    peakFinder.setFramesEnd(dataf->nFrames());

    peakFinder.find(numors);
    auto found_peaks = peakFinder.currentPeaks();
    CHECK(found_peaks.size() == 1);

    auto qvec = found_peaks[0]->q();
    std::cout << "q = " << qvec << std::endl;

    nsx::Ellipsoid elli_real = found_peaks[0]->shape();
    nsx::Ellipsoid elli_recip = found_peaks[0]->qShape();

    std::cout << "\nreal ellipse center:\n" << elli_real.center() << std::endl;
    std::cout << "\nreal ellipse metric:\n" << elli_real.metric() << std::endl;
    std::cout << "\nrecip ellipse center:\n" << elli_recip.center() << std::endl;
    std::cout << "\nrecip ellipse metric:\n" << elli_recip.metric() << std::endl;

    for (int i = 0; i < 3; ++i)
        CHECK(qvec[i] == Approx(elli_recip.center()[i]).epsilon(1e-3));

    dataf->close();
}
