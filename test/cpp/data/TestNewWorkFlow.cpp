#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "base/geometry/DirectVector.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/algo/DataReaderFactory.h"
#include "core/analyse/PeakFilter.h"
#include "core/analyse/PeakFinder.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/shape/ShapeLibrary.h"

TEST_CASE("test/data/TestNewWorkFlow.cpp", "")
{
    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.diffractometer()));

    experiment.addData(dataf);

    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);


    auto callback = [progressHandler]() {
        auto log = progressHandler->getLog();
        for (auto&& msg : log)
            std::cout << msg << std::endl;
    };

    progressHandler->setCallback(callback);

    // #########################################################
    // test the finder
    nsx::DataList numors;
    numors.push_back(dataf);

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular", {});

    nsx::PeakFinder* peak_finder = experiment.peakFinder();
    peak_finder->setMinSize(30);
    peak_finder->setMaxSize(10000);
    peak_finder->setPeakScale(1.0);
    peak_finder->setMaxFrames(10);
    peak_finder->setFramesBegin(0);
    peak_finder->setFramesEnd(dataf->nFrames());
    peak_finder->setThreshold(10);
    peak_finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
    peak_finder->setHandler(progressHandler);
    peak_finder->find(numors);

    auto found_peaks = peak_finder->currentPeaks();

    try {
        CHECK(static_cast<int>(found_peaks.size()) >= 0);
    } catch (...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    CHECK(found_peaks.size() >= 800);

    nsx::IPeakIntegrator* integrator = experiment.getIntegrator(
        std::string("Pixel sum integrator"));
    integrator->setPeakEnd(3.0);
    integrator->setBkgBegin(3.5);
    integrator->setBkgEnd(4.0);
    integrator->setHandler(progressHandler);
    experiment.integrateFoundPeaks("Pixel sum integrator");
    experiment.acceptFoundPeaks("found_peaks");

    // #########################################################
    // Filter the peaks
    nsx::PeakFilter* peak_filter = experiment.peakFilter();
    std::bitset<13> booleans;
    booleans.set(10);
    const std::array<double, 2> d_range {1.5,50};
    peak_filter->setBooleans(booleans);
    peak_filter->setDRange(d_range);

    nsx::PeakCollection* found_collection = experiment.getPeakCollection(
        "found_peaks");
    peak_filter->resetFiltering(found_collection);
    peak_filter->filter(found_collection);

    experiment.acceptFilter("filtered_peaks", found_collection);

    CHECK(experiment.getPeakCollection(
        "filtered_peaks")->getPeakList().size() >= 100);

    // #########################################################
    // at this stage we have the peaks, now we index
    nsx::AutoIndexer* auto_indexer = experiment.autoIndexer();
    nsx::PeakCollection* filtered_peaks = experiment.getPeakCollection(
        "filtered_peaks");

    nsx::IndexerParameters parameters;
    auto_indexer->setParameters(parameters);

    CHECK_NOTHROW(auto_indexer->autoIndex(filtered_peaks));
    CHECK(auto_indexer->solutions().size() > 1);

    auto solution = auto_indexer->solutions().front();

    // correctly indexed at least 92% of peaks
    CHECK(solution.second > 92.0);

    // set unit cell
    auto cell = solution.first;
    for (auto&& peak : filtered_peaks->getPeakList())
        peak->setUnitCell(cell);

    // reintegrate peaks
    integrator->setPeakEnd(3.0);
    integrator->setBkgBegin(4.0);
    integrator->setBkgEnd(5.0);
    experiment.integrateFoundPeaks("Pixel sum integrator");

    // #########################################################
    // compute shape library
    int n_selected = 0;
    for (auto peak : filtered_peaks->getPeakList()) {
        std::vector<nsx::ReciprocalVector> q_vectors;
        q_vectors.push_back(peak->q());
        auto events = dataf->events(q_vectors);

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
