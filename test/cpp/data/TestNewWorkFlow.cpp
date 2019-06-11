#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "core/indexing/AutoIndexer.h"
#include "core/detector/DetectorEvent.h"
#include "core/peak/Peak3D.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/geometry/DirectVector.h"
#include "core/geometry/ReciprocalVector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/ShapeLibrary.h"
#include "core/experiment/DataReaderFactory.h"
#include "core/peak/Peak3D.h"
#include "core/merge_and_filter/PeakFilter.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/utils/ProgressHandler.h"
#include "core/utils/Units.h"

TEST_CASE("test/data/TestNewWorkFlow.cpp", "")
{

    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.diffractometer()));

    experiment.addData(dataf);

    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);
    nsx::sptrPeakFinder peakFinder(new nsx::PeakFinder);

    auto callback = [progressHandler]() {
        auto log = progressHandler->getLog();
        for (auto&& msg : log)
            std::cout << msg << std::endl;
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

    nsx::PixelSumIntegrator integrator(false, false);
    integrator.setHandler(progressHandler);
    integrator.integrate(found_peaks, dataf, 2.7, 3.5, 4.0);

    // at this stage we have the peaks, now we index
    nsx::IndexerParameters params;
    nsx::AutoIndexer indexer(progressHandler);

    nsx::PeakFilter peak_filter;
    nsx::PeakList selected_peaks;
    selected_peaks = peak_filter.enabled(found_peaks, true);

    auto numIndexedPeaks = [&]() -> unsigned int {
        unsigned int indexed_peaks = 0;

        for (auto&& peak : selected_peaks) {
            double d = 1.0 / peak->q().rowVector().norm();

            if (d < 2.0)
                continue;

            indexer.addPeak(peak);
            ++indexed_peaks;
        }
        return indexed_peaks;
    };

    unsigned int indexed_peaks = numIndexedPeaks();

    CHECK(indexed_peaks > 500);
    CHECK_NOTHROW(indexer.autoIndex(params));
    CHECK(indexer.solutions().size() > 1);

    auto soln = indexer.solutions().front();

    // correctly indexed at least 92% of peaks
    CHECK(soln.second > 92.0);

    // set unit cell
    auto cell = soln.first;
    for (auto&& peak : found_peaks)
        peak->setUnitCell(cell);

    // reintegrate peaks
    integrator.integrate(found_peaks, dataf, 3.0, 4.0, 5.0);

    // compute shape library

    // dataf->integratePeaks(found_peaks, integrator, 3.0, 4.0, 5.0);

    indexed_peaks = numIndexedPeaks();
    std::cout << indexed_peaks << std::endl;
    CHECK(indexed_peaks > 500);

    int n_selected = 0;

    for (auto peak : selected_peaks) {

        std::vector<nsx::ReciprocalVector> q_vectors;
        q_vectors.push_back(peak->q());
        auto events = dataf->events(q_vectors);

        // CHECK(events.size() >= 1);

        if (events.size() == 0)
            continue;

        ++n_selected;

        Eigen::Vector3d p0 = peak->shape().center();
        Eigen::Vector3d p1;

        double diff = 1e200;

        // q could cross Ewald sphere multiple times, so find best match
        for (auto&& event : events) {
            const Eigen::Vector3d pnew = {event._px, event._py, event._frame};
            if ((pnew - p0).squaredNorm() < diff) {
                diff = (pnew - p0).squaredNorm();
                p1 = pnew;
            }
        }

        Eigen::RowVector3d q0 = nsx::Peak3D(dataf, nsx::Ellipsoid(p0, 1.0)).q().rowVector();
        Eigen::RowVector3d q1 = nsx::Peak3D(dataf, nsx::Ellipsoid(p1, 1.0)).q().rowVector();

        CHECK(p0(0) == Approx(p1(0)).epsilon(3e-2));
        CHECK(p0(1) == Approx(p1(1)).epsilon(3e-2));
        CHECK(p0(2) == Approx(p1(2)).epsilon(3e-2));

        CHECK(q0(0) == Approx(q1(0)).epsilon(2e-2));
        CHECK(q0(1) == Approx(q1(1)).epsilon(2e-2));
        CHECK(q0(2) == Approx(q1(2)).epsilon(2e-2));
    }

    CHECK(n_selected > 600);
}
