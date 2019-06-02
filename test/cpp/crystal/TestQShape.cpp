#include "test/cpp/catch.hpp"
#include <Eigen/Dense>

#include "core/search_peaks/ConvolverFactory.h"
#include "core/data/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Experiment.h"
#include "core/crystal/Peak3D.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/utils/ProgressHandler.h"
#include "core/rec_space/ReciprocalVector.h"
#include "core/instrument/Sample.h"
#include "core/utils/Units.h"

TEST_CASE("test/crystal/TestQShape.cpp", "") {

    nsx::DataReaderFactory factory;
    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.diffractometer()));
    experiment.addData(dataf);

    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);
    nsx::sptrPeakFinder peakFinder(new nsx::PeakFinder);

    auto callback = [progressHandler]() {
        auto log = progressHandler->getLog();
        for (auto&& msg : log) {
            std::cout << msg << std::endl;
        }
    };

    progressHandler->setCallback(callback);

    nsx::DataList numors;
    numors.push_back(dataf);

    // propagate changes to peak finder
    peakFinder->setMinSize(30);
    peakFinder->setMaxSize(10000);
    peakFinder->setMaxFrames(10);

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular", {});
    peakFinder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));

    peakFinder->setThreshold(15.0);
    peakFinder->setPeakScale(1.0);

    peakFinder->setHandler(progressHandler);

    auto found_peaks = peakFinder->find(numors);

    try {
        CHECK(static_cast<int>(found_peaks.size()) >= 0);
    } catch (...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    CHECK(found_peaks.size() >= 800);

    int good_shapes = 0;

    for (auto peak : found_peaks) {
        if (!peak->enabled()) {
            continue;
        }

        auto qshape = peak->qShape();
        nsx::Ellipsoid new_shape;
        try {
            new_shape = qshape.toDetectorSpace(dataf);
        } catch (...) {
            continue;
        }
        auto old_shape = peak->shape();

        // note: some blobs are invalid, so we skip them
        if (!(old_shape.metric().norm() < 1e3)) {
            continue;
        }

        ++good_shapes;

        auto dx = new_shape.center() - old_shape.center();

        // transformation x -> q -> x should have sub-pixel accuracy
        CHECK(std::abs(dx.norm()) < 0.01);

        double error = (new_shape.metric() - old_shape.metric()).norm();
        CHECK(std::abs(error) < 2e-2);
    }

    CHECK(good_shapes > 600);
}
