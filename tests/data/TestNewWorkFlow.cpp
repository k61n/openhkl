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
#include <nsxlib/Units.h>

int main()
{
    nsx::ShapeLibrary library;
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
    peakFinder->setSearchConfidence(0.98);
    peakFinder->setIntegrationConfidence(0.997);
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

    NSX_CHECK_ASSERT(indexed_peaks > 650);
    NSX_CHECK_NO_THROW(indexer.autoIndex(params));

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
    const double scale = nsx::getScale(0.997);
    // todo: bkg_begin and bkg_end
    dataf->integratePeaks(found_peaks, scale, 2.0*scale);

    indexed_peaks = numIndexedPeaks();
    NSX_CHECK_ASSERT(indexed_peaks > 600);

    Eigen::Matrix3d q_cov;
    int n_selected = 0;
    q_cov.setZero();

    // get that DataSet::getEvents works properly
    for (auto peak: selected_peaks) {

        std::vector<nsx::ReciprocalVector> q_vectors;
        q_vectors.push_back(peak->q());
        auto events = dataf->getEvents(q_vectors);

        NSX_CHECK_ASSERT(events.size() >= 1);

        if (events.size() == 0) {
            continue;
        }

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

        NSX_CHECK_CLOSE(q0(0), q1(0), 1.0);
        NSX_CHECK_CLOSE(q0(1), q1(1), 1.0);
        NSX_CHECK_CLOSE(q0(2), q1(2), 1.0);

        auto q_shape = peak->qShape();
        nsx::MillerIndex hkl(peak->q(), *cell);
        library.addShape(hkl, q_shape.inverseMetric());
        q_cov += q_shape.inverseMetric();
        n_selected += 1;
    }

    q_cov /= n_selected;
    library.setDefaultShape(q_cov);

    nsx::PeakPredictor predictor(cell, library, 2.1, 50.0, 4);
    auto predicted_peaks = predictor.predict(dataf);

    std::cout << "predicted_peaks: " << predicted_peaks.size() << std::endl;
    NSX_CHECK_ASSERT(predicted_peaks.size() > 1600);

    return 0;
}
