#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/AutoIndexer.h>
#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>

#include <nsxlib/Experiment.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/Refiner.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Units.h>

NSX_INIT_TEST

int main()
{
    nsx::DataReaderFactory factory;

    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->diffractometer();
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", diff));

    expt->addData(dataf);

    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);
    nsx::sptrPeakFinder peakFinder(new nsx::PeakFinder);

    auto callback = [progressHandler] () {
        auto log = progressHandler->getLog();
        for (auto&& msg: log) {
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
    peakFinder->setConvolver("annular",{});
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
    
    nsx::Refiner refiner(cell, peaks, 1);

    NSX_CHECK_ASSERT(refiner.batches().size() == 1);

    for (auto&& batch: refiner.batches()) {
        NSX_CHECK_ASSERT(batch.peaks().size() > 200);
    }

    refiner.refineB();
    //refiner.refineU();
    auto&& states = dataf->instrumentStates();
    refiner.refineSamplePosition(states);

    std::cout << "peaks to refine: " << peaks.size() << std::endl;

    NSX_CHECK_ASSERT(refiner.refine(500));
    return 0;
}
