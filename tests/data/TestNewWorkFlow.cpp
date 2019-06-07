#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <core/AutoIndexer.h>
#include <core/ConvolverFactory.h>
#include <core/CrystalTypes.h>
#include <core/DataReaderFactory.h>
#include <core/DataSet.h>
#include <core/DetectorEvent.h>
#include <core/Diffractometer.h>
#include <core/DirectVector.h>
#include <core/Experiment.h>
#include <core/NSXTest.h>
#include <core/Peak3D.h>
#include <core/PeakFilter.h>
#include <core/PeakFinder.h>
#include <core/PixelSumIntegrator.h>
#include <core/ProgressHandler.h>
#include <core/ReciprocalVector.h>
#include <core/Sample.h>
#include <core/ShapeLibrary.h>
#include <core/Units.h>

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

    try {
        NSX_CHECK_ASSERT(static_cast<int>(found_peaks.size()) >= 0);
    } catch (...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    NSX_CHECK_ASSERT(found_peaks.size() >= 800);

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

            if (d < 2.0) {
                continue;
            }

            indexer.addPeak(peak);
            ++indexed_peaks;
        }
        return indexed_peaks;
    };

    unsigned int indexed_peaks = numIndexedPeaks();

    NSX_CHECK_ASSERT(indexed_peaks > 500);
    NSX_CHECK_NO_THROW(indexer.autoIndex(params));
    NSX_CHECK_ASSERT(indexer.solutions().size() > 1);

    auto soln = indexer.solutions().front();

    // correctly indexed at least 92% of peaks
    NSX_CHECK_ASSERT(soln.second > 92.0);

    // set unit cell
    auto cell = soln.first;
    for (auto&& peak : found_peaks) {
        peak->setUnitCell(cell);
    }

    // reintegrate peaks
    integrator.integrate(found_peaks, dataf, 3.0, 4.0, 5.0);

    // compute shape library

    // dataf->integratePeaks(found_peaks, integrator, 3.0, 4.0, 5.0);

    indexed_peaks = numIndexedPeaks();
    std::cout << indexed_peaks << std::endl;
    NSX_CHECK_ASSERT(indexed_peaks > 500);

    int n_selected = 0;

    for (auto peak : selected_peaks) {

        std::vector<nsx::ReciprocalVector> q_vectors;
        q_vectors.push_back(peak->q());
        auto events = dataf->events(q_vectors);

        // NSX_CHECK_ASSERT(events.size() >= 1);

        if (events.size() == 0) {
            continue;
        }

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

        NSX_CHECK_CLOSE(p0(0), p1(0), 3.0);
        NSX_CHECK_CLOSE(p0(1), p1(1), 3.0);
        NSX_CHECK_CLOSE(p0(2), p1(2), 3.0);

        NSX_CHECK_CLOSE(q0(0), q1(0), 2.0);
        NSX_CHECK_CLOSE(q0(1), q1(1), 2.0);
        NSX_CHECK_CLOSE(q0(2), q1(2), 2.0);
    }

    NSX_CHECK_GREATER_THAN(n_selected, 600);

    return 0;
}
