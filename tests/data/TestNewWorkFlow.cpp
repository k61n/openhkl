#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/AutoIndexer.h>
#include <nsxlib/ConvolutionKernel.h>
#include <nsxlib/CrystalTypes.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/DirectVector.h>
#include <nsxlib/ErfInv.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/KernelFactory.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Sample.h>
#include <nsxlib/ShapeLibrary.h>
#include <nsxlib/StrongPeakIntegrator.h>
#include <nsxlib/Units.h>

int main()
{
    nsx::DataReaderFactory factory;

    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
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

    nsx::sptrConvolutionKernel kernel;
    std::string kernelName = "annular";
    auto kernelFactory = nsx::KernelFactory::Instance();
    kernel.reset(kernelFactory->create(kernelName, int(dataf->nRows()), int(dataf->nCols())));

    // propagate changes to peak finder
    auto convolver = peakFinder->convolver();
    convolver->setKernel(kernel->matrix());
    peakFinder->setMinComponents(30);
    peakFinder->setMaxComponents(10000);
    peakFinder->setKernel(kernel);
    peakFinder->setSearchScale(1.5);
    peakFinder->setIntegrationScale(4.0);
    peakFinder->setBackgroundScale(6.0);
    peakFinder->setThresholdType(1); // absolute
    peakFinder->setThresholdValue(15.0);

    peakFinder->setHandler(progressHandler);

    auto found_peaks = peakFinder->find(numors);

    try {
        NSX_CHECK_ASSERT(static_cast<int>(found_peaks.size()) >= 0);
    } catch(...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    NSX_CHECK_ASSERT(found_peaks.size() >= 800);

    // at this stage we have the peaks, now we index
    nsx::IndexerParameters params;
    nsx::AutoIndexer indexer(progressHandler);

    nsx::PeakFilter peak_filter;
    nsx::PeakList selected_peaks;
    selected_peaks = peak_filter.selected(found_peaks,true);
    selected_peaks = peak_filter.dRange(selected_peaks, 2.0, 100.0, true);

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

    NSX_CHECK_ASSERT(indexer.getSolutions().empty() == false);

    auto soln = indexer.getSolutions().front();

    // correctly indexed at least 92% of peaks
    NSX_CHECK_ASSERT(soln.second > 92.0);

    // set unit cell
    auto cell = soln.first;
    for (auto&& peak: found_peaks) {
        peak->addUnitCell(cell, true);
    }

    // add cell to sample
    dataf->diffractometer()->getSample()->addUnitCell(cell);

    // reintegrate peaks
    nsx::StrongPeakIntegrator integrator;
    integrator.integrate(found_peaks, dataf, 3.0, 4.0, 5.0);

    // compute shape library

    //dataf->integratePeaks(found_peaks, integrator, 3.0, 4.0, 5.0);

    indexed_peaks = numIndexedPeaks();
    NSX_CHECK_ASSERT(indexed_peaks > 600);

    int n_selected = 0;

    // get that DataSet::getEvents works properly
    for (auto peak: selected_peaks) {

        std::vector<nsx::ReciprocalVector> q_vectors;
        q_vectors.push_back(peak->q());
        auto events = dataf->getEvents(q_vectors);

        //NSX_CHECK_ASSERT(events.size() >= 1);

        if (events.size() == 0) {
            continue;
        }

        ++n_selected;

        Eigen::Vector3d p0 = peak->getShape().center();
        Eigen::Vector3d p1;

        double diff = 1e200;

        // q could cross Ewald sphere multiple times, so find best match
        for (auto&& event: events) {
            const Eigen::Vector3d pnew = {event._px, event._py, event._frame};
            if ((pnew-p0).squaredNorm() < diff) {
                diff = (pnew-p0).squaredNorm();
                p1 = pnew;
            }
        }
        
        Eigen::RowVector3d q0 = nsx::Peak3D(dataf, nsx::Ellipsoid(p0, 1.0)).q().rowVector();
        Eigen::RowVector3d q1 = nsx::Peak3D(dataf, nsx::Ellipsoid(p1, 1.0)).q().rowVector();

        NSX_CHECK_CLOSE(p0(0), p1(0), 3.0);
        NSX_CHECK_CLOSE(p0(1), p1(1), 3.0);
        NSX_CHECK_CLOSE(p0(2), p1(2), 3.0);

        NSX_CHECK_CLOSE(q0(0), q1(0), 2.0);
        NSX_CHECK_CLOSE(q0(1), q1(1), 2.0);
        NSX_CHECK_CLOSE(q0(2), q1(2), 2.0);

        #if 0
        if (library.addPeak(peak)) {
            ++n_selected;
        } else {
            peak->setSelected(false);
        }
        #endif
    }

    NSX_CHECK_GREATER_THAN(n_selected, 600);

    // TODO: put peak prediction back into workflow test!!!

    #if 0
    NSX_CHECK_ASSERT(n_selected > 620);

    library.setDefaultShape(library.meanShape());

    nsx::PeakPredictor predictor(cell, library, 2.1, 50.0, 4);
    auto predicted_peaks = predictor.predict(dataf);

    std::cout << "predicted_peaks: " << predicted_peaks.size() << std::endl;
    NSX_CHECK_ASSERT(predicted_peaks.size() > 1600);
    #endif

    return 0;
}
