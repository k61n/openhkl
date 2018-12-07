#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <core/AutoIndexer.h>
#include <core/ConvolverFactory.h>
#include <core/CrystalTypes.h>
#include <core/DataSet.h>
#include <core/Diffractometer.h>
#include <core/Experiment.h>
#include <core/Gonio.h>
#include <core/InstrumentState.h>
#include <core/NSXTest.h>
#include <core/Peak3D.h>
#include <core/PeakFilter.h>
#include <core/PeakFinder.h>
#include <core/ProgressHandler.h>
#include <core/Refiner.h>
#include <core/ReciprocalVector.h>
#include <core/Sample.h>
#include <core/Units.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataset(new nsx::DataSet("hdf", "gal3.hdf", experiment.diffractometer()));

    experiment.addData(dataset);

    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);

    auto callback = [progressHandler] () {
        auto log = progressHandler->getLog();
        for (auto&& msg: log) {
            std::cout << msg << std::endl;
        }
    };

    progressHandler->setCallback(callback);

    nsx::DataList datasets;
    datasets.push_back(dataset);

    nsx::PeakFinder peakFinder(datasets);

    // propagate changes to peak finder
    peakFinder.setMinSize(30);
    peakFinder.setMaxSize(10000);
    peakFinder.setMaxFrames(10);

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular",{});

    peakFinder.setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
    peakFinder.setThreshold(15.0);
    peakFinder.setPeakMergingScale(1.0);

    peakFinder.run();

    auto found_peaks = peakFinder.peaks();

    NSX_CHECK_ASSERT(found_peaks.size() >= 800);

    // at this stage we have the peaks, now we index
    nsx::IndexerParameters params;
    nsx::AutoIndexer indexer(progressHandler);

    nsx::PeakFilter peak_filter;
    nsx::PeakList selected_peaks;
    selected_peaks = peak_filter.enabled(found_peaks,true);
    selected_peaks = peak_filter.dRange(selected_peaks, 2.0, 100.0);
    
    NSX_CHECK_ASSERT(selected_peaks.size() >= 600);

    auto numIndexedPeaks = [&]() -> unsigned int
    {
        unsigned int indexed_peaks = 0;

        for (auto&& peak: selected_peaks) {
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
    NSX_CHECK_SMALL( (cell->reciprocalBasis()-constrained_cell.reciprocalBasis()).norm(), 1e-6);

    for (auto&& peak: found_peaks) {
        peak->setUnitCell(cell);
        peaks.push_back(peak);
    }
    
    auto&& states = dataset->instrumentStates();

    nsx::Refiner refiner(states, cell, peaks, 1);

    NSX_CHECK_ASSERT(refiner.batches().size() == 1);

    for (auto&& batch: refiner.batches()) {
        NSX_CHECK_ASSERT(batch.peaks().size() > 200);
    }

    refiner.refineUB();
    refiner.refineSamplePosition();

    std::cout << "peaks to refine: " << peaks.size() << std::endl;

    NSX_CHECK_ASSERT(refiner.refine(500));

    dataset->close();

    return 0;
}
