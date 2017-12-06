#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/AutoIndexer.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/UBSolution.h>
#include <nsxlib/UBMinimizer.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/ConvolutionKernel.h>
#include <nsxlib/KernelFactory.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/Sample.h>
#include <nsxlib/ErfInv.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>

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
    kernel.reset(kernelFactory->create(kernelName, int(dataf->getNRows()), int(dataf->getNCols())));

    auto k = kernel->getKernel();

    // propagate changes to peak finder
    auto convolver = peakFinder->getConvolver();
    convolver->setKernel(kernel->getKernel());
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
        NSX_CHECK_ASSERT(found_peaks.size() >= 0);
    } catch(...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    NSX_CHECK_ASSERT(found_peaks.size() >= 800);

    int good_shapes = 0;
    nsx::PeakPredictor pred(dataf);

    for (auto peak: found_peaks) {
        try {
            auto qshape = peak->qShape();
            nsx::Ellipsoid new_shape;
            new_shape = pred.toDetectorSpace(qshape);
            auto old_shape = peak->getShape();

            // note: some blobs are invalid, so we skip them
            if (!(old_shape.metric().norm() < 1e3)) {
                continue;
            }

            ++good_shapes;

            auto dx = new_shape.center() - old_shape.center();

            // transformation x -> q -> x should be nearly pixel-perfect
            NSX_CHECK_SMALL(dx.norm(), 0.2);

            NSX_CHECK_SMALL((new_shape.metric().inverse()*old_shape.metric()-Eigen::Matrix3d::Identity()).norm(), 0.4);

            std::cout << "q\n" << qshape.metric() << "\n";
            std::cout << "new\n" << new_shape.metric() << "\n--------------------------\n";
            std::cout << old_shape.metric() << std::endl;
            std::cout << "compared to identity\n";
            std::cout << new_shape.metric().inverse()*old_shape.metric()-Eigen::Matrix3d::Identity() << "\n=====================================" << std::endl;
        } catch (...) {
            
        }
    }

    std::cout << "good_shapes " << good_shapes << std::endl;

    return 0;
}
