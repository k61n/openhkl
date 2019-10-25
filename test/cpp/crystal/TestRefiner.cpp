#include "test/cpp/catch.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "core/algo/AutoIndexer.h"
#include "core/algo/DataReaderFactory.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "core/peak/IPeakIntegrator.h"

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"
#include "core/algo/Refiner.h"
#include "core/analyse/PeakFilter.h"
#include "core/analyse/PeakFinder.h"
#include "core/experiment/Experiment.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/peak/Peak3D.h"

TEST_CASE("test/crystal/TestRefiner.cpp", "")
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
    peak_finder->setThreshold(15);
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

    integrator->setPeakEnd(2.7);
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
    const std::array<double, 2> d_range {1.5, 50};
    peak_filter->setBooleans(booleans);
    peak_filter->setDRange(d_range);

    nsx::PeakCollection* found_collection = experiment.getPeakCollection(
        "found_peaks");
    peak_filter->resetFiltering(found_collection);
    peak_filter->filter(found_collection);

    experiment.acceptFilter("filtered_peaks", found_collection);

    CHECK(experiment.getPeakCollection(
        "filtered_peaks")->getPeakList().size() >= 600);

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

    // correctly indexed at least 98% of peaks
    CHECK(solution.second > 98.0);

    

    // set unit cell
    auto cell = solution.first;

    // set constraints
    auto constrained_cell = cell->applyNiggliConstraints();
    CHECK(std::abs((cell->reciprocalBasis() - constrained_cell.reciprocalBasis()).norm()) < 1e-6);

    std::vector<nsx::Peak3D*> peaks;
    for (auto&& peak : filtered_peaks->getPeakList()){
        peak->setUnitCell(cell);
        peaks.push_back(peak);
    }

    auto&& states = dataf->instrumentStates();
    nsx::Refiner refiner(states, cell.get(), peaks, 1);

    CHECK(refiner.batches().size() == 1);

    for (auto&& batch : refiner.batches())
        CHECK(batch.peaks().size() > 200);

    refiner.refineUB();
    refiner.refineSamplePosition();

    std::cout << "peaks to refine: " << peaks.size() << std::endl;

    CHECK(refiner.refine(500));
}
