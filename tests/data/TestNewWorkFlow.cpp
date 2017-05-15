#define BOOST_TEST_MODULE "Test New Work Flow"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/utils/Units.h>
#include <nsxlib/utils/erf_inv.h>
#include <nsxlib/utils/ProgressHandler.h>
#include <nsxlib/data/PeakFinder.h>
#include <nsxlib/imaging/ConvolutionKernel.h>
#include <nsxlib/imaging/KernelFactory.h>
#include <nsxlib/crystal/AutoIndexer.h>
#include <nsxlib/instrument/Experiment.h>

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

using SX::Utils::ProgressHandler;
using SX::Data::PeakFinder;
using SX::Imaging::ConvolutionKernel;
using SX::Imaging::KernelFactory;
using SX::Crystal::AutoIndexer;
using SX::Crystal::UnitCell;
using SX::Instrument::Experiment;

// const double tolerance=1e-2;

int run_test();

BOOST_AUTO_TEST_CASE(Test_NewWorkFlow)
{
    BOOST_CHECK_EQUAL(run_test(), 0);
}

int run_test()
{
    DataReaderFactory* factory = DataReaderFactory::Instance();
    DiffractometerStore* ds = DiffractometerStore::Instance();

    std::shared_ptr<Experiment> expt(new Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
    std::shared_ptr<DataSet> dataf(factory->create("hdf", "gal3.hdf", diff));

    expt->addData(dataf);

    std::shared_ptr<ProgressHandler> progressHandler(new ProgressHandler);
    std::shared_ptr<PeakFinder> peakFinder(new PeakFinder);

    auto callback = [progressHandler] () {
        auto log = progressHandler->getLog();
        for (auto&& msg: log) {
            std::cout << msg << std::endl;
        }
    };

    progressHandler->setCallback(callback);

    std::vector<std::shared_ptr<DataSet>> numors;
    numors.push_back(dataf);

    std::shared_ptr<SX::Imaging::ConvolutionKernel> kernel;
    std::string kernelName = "annular";
    SX::Imaging::KernelFactory* kernelFactory = SX::Imaging::KernelFactory::Instance();
    kernel.reset(kernelFactory->create(kernelName, int(dataf->getNRows()), int(dataf->getNCols())));

    auto k = kernel->getKernel();
    double norm2 = (k*k.transpose()).sum();
    std::cout << "norm 2" << norm2 << std::endl;

    // propagate changes to peak finder
    auto convolver = peakFinder->getConvolver();
    convolver->setKernel(kernel->getKernel());
    peakFinder->setMinComponents(30);
    peakFinder->setMaxComponents(10000);
    peakFinder->setKernel(kernel);
    peakFinder->setConfidence(0.98);
    peakFinder->setThresholdType(1); // absolute
    peakFinder->setThresholdValue(15.0);

    peakFinder->setHandler(progressHandler);

    try {
        BOOST_CHECK(peakFinder->find(numors) == true);
    } catch(...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    std::cout << dataf->getPeaks().size() << std::endl;
    BOOST_CHECK(dataf->getPeaks().size() >= 800);

    // at this stage we have the peaks, now we index
    AutoIndexer::Parameters params;
    AutoIndexer indexer(expt, progressHandler);

    auto numIndexedPeaks = [&]() -> unsigned int
    {
        unsigned int indexed_peaks = 0;

        for (auto&& peak: dataf->getPeaks()) {
            if (!peak->isSelected() || peak->isMasked()) {
                continue;
            }
            ++indexed_peaks;
            indexer.addPeak(peak);
        }
        return indexed_peaks;
    };

    unsigned int indexed_peaks = numIndexedPeaks();

    BOOST_CHECK(indexed_peaks > 650);
    BOOST_CHECK(indexer.autoIndex(params));

    auto soln = indexer.getSolutions().front();

    // correctly indexed at least 92% of peaks
    BOOST_CHECK(soln.second > 92.0);

    // set unit cell
    auto cell = std::make_shared<UnitCell>(soln.first);
    for (auto&& peak: dataf->getPeaks()) {
        peak->addUnitCell(cell, true);
    }

    // reintegrate peaks
    const double scale = SX::Utils::getScale(0.997);
    dataf->integratePeaks(scale, 2.0*scale, true);

    indexed_peaks = numIndexedPeaks();
    std::cout << indexed_peaks << std::endl;
    BOOST_CHECK(indexed_peaks > 600);

    return 0;
}
