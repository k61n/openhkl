
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/data/FrameInterval.h>
#include <nsxlib/imaging/KernelFactory.h>
#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/utils/ProgressHandler.h>
#include <nsxlib/utils/NSXTest.h>

int main()
{
    nsx::DataReaderFactory factory;

    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", diff));

    expt->addData(dataf);

    nsx::sptrPeakFinder peakFinder(new nsx::PeakFinder);

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

    peakFinder->find(numors);

    int n_peaks_found = dataf->getPeaks().size();

    nsx::SimpleFrameInterval interval(dataf);

    NSX_CHECK_EQUAL(interval.peaks().size(),n_peaks_found);

    nsx::SimpleFrameInterval interval1(dataf,0,30);
    NSX_CHECK_EQUAL(interval1.peaks().size(),n_peaks_found);

    return 0;
}
