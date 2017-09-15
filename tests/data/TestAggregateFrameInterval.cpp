#define BOOST_TEST_MODULE "TestAggregateFrameInterval"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/data/AggregateFrameInterval.h>
#include <nsxlib/data/SimpleFrameInterval.h>
#include <nsxlib/imaging/KernelFactory.h>
#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/utils/ProgressHandler.h>

int run_test();

BOOST_AUTO_TEST_CASE(Test_AggregateFrameInterval)
{
    BOOST_CHECK_EQUAL(run_test(), 0);
}

int run_test()
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

    nsx::IFrameInterval* interval_all = new nsx::SimpleFrameInterval(dataf,0,30);
    nsx::AggregateFrameInterval interval(dataf);
    interval.addInterval(interval_all);

    BOOST_CHECK_EQUAL(interval.peaks().size(),n_peaks_found);

    nsx::AggregateFrameInterval interval1(dataf);

    nsx::IFrameInterval* interval_00_10 = new nsx::SimpleFrameInterval(dataf, 0,10);
    nsx::IFrameInterval* interval_10_20 = new nsx::SimpleFrameInterval(dataf,10,20);
    nsx::IFrameInterval* interval_20_30 = new nsx::SimpleFrameInterval(dataf,20,30);

    interval1.addInterval(interval_00_10);
    interval1.addInterval(interval_10_20);
    interval1.addInterval(interval_20_30);

    BOOST_CHECK_EQUAL(interval1.peaks().size(),n_peaks_found);

    return 0;
}
