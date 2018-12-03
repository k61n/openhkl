#include <Eigen/Dense>

#include <nsxlib/ConvolverFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Units.h>

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

    try {
        NSX_CHECK_ASSERT(static_cast<int>(found_peaks.size()) >= 0);
    } catch(...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    NSX_CHECK_ASSERT(found_peaks.size() >= 800);

    int good_shapes = 0;

    for (auto peak: found_peaks) {
        if (!peak->enabled()) {
            continue;
        }

        auto qshape = peak->qShape();
        nsx::Ellipsoid new_shape;
        try {
            new_shape = qshape.toDetectorSpace(dataset);
        } catch(...) {
            continue;
        }
        auto old_shape = peak->shape();

        // note: some blobs are invalid, so we skip them
        if (!(old_shape.metric().norm() < 1e3)) {
            continue;
        }

        ++good_shapes;

        auto dx = new_shape.center() - old_shape.center();
    
        // transformation x -> q -> x should have sub-pixel accuracy
        NSX_CHECK_SMALL(dx.norm(), 0.01);

        double error = (new_shape.metric()-old_shape.metric()).norm();
        NSX_CHECK_SMALL(error, 2e-2);
       
    }

    NSX_CHECK_ASSERT(good_shapes > 600);

    return 0;
}
