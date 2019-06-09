#include "test/cpp/catch.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "core/auto_indexing/AutoIndexer.h"
#include "core/experiment/CrystalTypes.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/DataReaderFactory.h"
#include "core/search_peaks/ConvolverFactory.h"

#include "core/experiment/Experiment.h"
#include "core/geometry/ReciprocalVector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakFilter.h"
#include "core/refine/Refiner.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/utils/ProgressHandler.h"
#include "core/utils/Units.h"

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
    CHECK(found_peaks.size() >= 800);

    // at this stage we have the peaks, now we index
    nsx::AutoIndexer indexer(progressHandler);

    nsx::PeakFilter peak_filter;
    nsx::PeakList selected_peaks;
    selected_peaks = peak_filter.enabled(found_peaks, true);
    selected_peaks = peak_filter.dRange(selected_peaks, 2.0, 100.0);

    CHECK(selected_peaks.size() >= 600);

    unsigned int indexed_peaks = 0;
    for (auto&& peak : selected_peaks) {
        indexer.addPeak(peak);
        ++indexed_peaks;
    }
    CHECK(indexed_peaks > 600);

    nsx::IndexerParameters params;
    CHECK_NOTHROW(indexer.autoIndex(params));

    if (indexer.solutions().empty()) {
        std::cerr << "AutoIndexer::autoIndex found no solution\n";
        CHECK(false);
        return;
    }

    auto soln = indexer.solutions().front();

    // correctly indexed at least 98% of peaks
    CHECK(soln.second > 98.0);

    nsx::PeakList peaks;

    // set unit cell
    auto cell = soln.first;

    // set constraints
    auto constrained_cell = cell->applyNiggliConstraints();
    CHECK(std::abs((cell->reciprocalBasis() - constrained_cell.reciprocalBasis()).norm()) < 1e-6);

    for (auto&& peak : found_peaks) {
        peak->setUnitCell(cell);
        peaks.push_back(peak);
    }

    auto&& states = dataf->instrumentStates();

    nsx::Refiner refiner(states, cell, peaks, 1);

    CHECK(refiner.batches().size() == 1);

    for (auto&& batch : refiner.batches())
        CHECK(batch.peaks().size() > 200);

    refiner.refineUB();
    refiner.refineSamplePosition();

    std::cout << "peaks to refine: " << peaks.size() << std::endl;

    CHECK(refiner.refine(500));
}
