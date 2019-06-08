#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "core/auto_indexing/AutoIndexer.h"
#include "core/search_peaks/ConvolverFactory.h"
#include "core/crystal/CrystalTypes.h"
#include "core/data/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"

#include "core/instrument/Experiment.h"
#include "core/axes/Gonio.h"
#include "core/instrument/InstrumentState.h"
#include "core/crystal/Peak3D.h"
#include "core/crystal/PeakFilter.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/utils/ProgressHandler.h"
#include "core/rec_space/ReciprocalVector.h"
#include "core/refine/Refiner.h"
#include "core/instrument/Sample.h"
#include "core/utils/Units.h"

NSX_INIT_TEST

int main()
{
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
    NSX_CHECK_ASSERT(found_peaks.size() >= 800);

    // at this stage we have the peaks, now we index
    nsx::IndexerParameters params;
    nsx::AutoIndexer indexer(progressHandler);

    nsx::PeakFilter peak_filter;
    nsx::PeakList selected_peaks;
    selected_peaks = peak_filter.enabled(found_peaks, true);
    selected_peaks = peak_filter.dRange(selected_peaks, 2.0, 100.0);

    NSX_CHECK_ASSERT(selected_peaks.size() >= 600);

    auto numIndexedPeaks = [&]() -> unsigned int {
        unsigned int indexed_peaks = 0;

        for (auto&& peak : selected_peaks) {
            indexer.addPeak(peak);
            ++indexed_peaks;
        }
        return indexed_peaks;
    };

    unsigned int indexed_peaks = numIndexedPeaks();

    NSX_CHECK_ASSERT(indexed_peaks > 600);
    NSX_CHECK_NO_THROW(indexer.autoIndex(params));

    NSX_CHECK_ASSERT(indexer.solutions().empty() == false);

    auto soln = indexer.solutions().front();

    // correctly indexed at least 98% of peaks
    NSX_CHECK_ASSERT(soln.second > 98.0);

    nsx::PeakList peaks;

    // set unit cell
    auto cell = soln.first;

    // set constraints
    auto constrained_cell = cell->applyNiggliConstraints();
    NSX_CHECK_SMALL((cell->reciprocalBasis() - constrained_cell.reciprocalBasis()).norm(), 1e-6);

    for (auto&& peak : found_peaks) {
        peak->setUnitCell(cell);
        peaks.push_back(peak);
    }

    auto&& states = dataf->instrumentStates();

    nsx::Refiner refiner(states, cell, peaks, 1);

    NSX_CHECK_ASSERT(refiner.batches().size() == 1);

    for (auto&& batch : refiner.batches()) {
        NSX_CHECK_ASSERT(batch.peaks().size() > 200);
    }

    refiner.refineUB();
    refiner.refineSamplePosition();

    std::cout << "peaks to refine: " << peaks.size() << std::endl;

    NSX_CHECK_ASSERT(refiner.refine(500));
    return 0;
}
