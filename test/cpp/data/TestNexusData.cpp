//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/data/TestNexusData.cpp
//! @brief     Tests the Nexus loader
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <vector>

#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/experiment/Experiment.h"
#include "base/utils/ProgressHandler.h"
#include "core/convolve/ConvolverFactory.h"


#define OUTPUT_INTERMEDIATE
#define ONLY_FIRST_FILE


TEST_CASE("test/data/TestNexusData.cpp", "")
{
    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);
    progressHandler->setCallback([progressHandler]() {
        auto log = progressHandler->getLog();
        for (auto&& msg : log)
            std::cerr << msg << std::endl;
    });



    std::cout << "\n--------------------------------------------------------------------------------" << std::endl;
    std::cout << "Loading files..." << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    nsx::DataReaderFactory factory;
    nsx::DataList numors;

    std::vector<std::string> files = {
        "/home/tw/tmp/nsx/internal-192/501168.nxs", "/home/tw/tmp/nsx/internal-192/501169.nxs",
        "/home/tw/tmp/nsx/internal-192/501170.nxs", "/home/tw/tmp/nsx/internal-192/501171.nxs",
        "/home/tw/tmp/nsx/internal-192/501172.nxs", "/home/tw/tmp/nsx/internal-192/501173.nxs",
        "/home/tw/tmp/nsx/internal-192/501174.nxs", "/home/tw/tmp/nsx/internal-192/501175.nxs",
        "/home/tw/tmp/nsx/internal-192/501176.nxs", "/home/tw/tmp/nsx/internal-192/501177.nxs",
        "/home/tw/tmp/nsx/internal-192/501178.nxs", "/home/tw/tmp/nsx/internal-192/501179.nxs",
        "/home/tw/tmp/nsx/internal-192/501180.nxs", "/home/tw/tmp/nsx/internal-192/501181.nxs",
        "/home/tw/tmp/nsx/internal-192/501182.nxs", "/home/tw/tmp/nsx/internal-192/501183.nxs",
        "/home/tw/tmp/nsx/internal-192/501184.nxs", "/home/tw/tmp/nsx/internal-192/501185.nxs",
    };

    nsx::Diffractometer* diffractometer = nsx::Diffractometer::create("D19");
    nsx::Experiment exp("test", diffractometer->name());

    std::size_t numframes = 0;
    for (const std::string& file : files) {
        std::cout << "Loading " << file << "..." << std::endl;

        std::ifstream ifstr(file);
        if (!ifstr.is_open()) {
            std::cerr << "Skipping test because the data files are not available" << std::endl;
            return;
        }

        std::shared_ptr<nsx::DataSet> datafile = factory.create("nxs", file, diffractometer);
        datafile->open();
        //std::cout << datafile->nFrames() << std::endl;

        std::cout << "Number of frames: " << datafile->nFrames() << std::endl;
        numframes += datafile->nFrames();
        for(std::size_t frame=0; frame<datafile->nFrames(); ++frame)
        {
            std::cout << "sample angles: ";
            for(std::size_t gonio=0; gonio<diffractometer->sample().gonio().nAxes(); ++gonio)
            {
                Eigen::MatrixXi M = datafile->frame(frame);
                CHECK((M.rows() == 256 && M.cols() == 640));

                std::cout << (datafile->reader()->sampleStates()[frame][gonio] / M_PI * 180.) << ", ";
            }
            std::cout << std::endl;
            break;  // only check first in loop
        }

        numors.push_back(datafile);
        exp.addData(datafile);

#ifdef ONLY_FIRST_FILE
        break;
#endif
    }

    std::cout << "Total number of frames: " << numframes << std::endl;
    std::cout << "--------------------------------------------------------------------------------\n" << std::endl;




    std::cout << "\n--------------------------------------------------------------------------------" << std::endl;
    std::cout << "Finding peaks..." << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular", {});

    nsx::PeakFinder* peak_finder = exp.peakFinder();
    peak_finder->setFramesBegin(0);
    peak_finder->setFramesEnd(numframes);
    peak_finder->setMaxFrames(numframes);

    peak_finder->setMinSize(30);
    peak_finder->setMaxSize(10000);
    peak_finder->setPeakScale(1.0);
    peak_finder->setThreshold(10);
    peak_finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
    peak_finder->setHandler(progressHandler);
    peak_finder->find(numors);

    auto found_peaks = peak_finder->currentPeaks();

#ifdef OUTPUT_INTERMEDIATE
    for (const nsx::Peak3D* pk : found_peaks) {
        if (!pk->enabled())
            continue;

        nsx::Ellipsoid elli_real = pk->shape();
        nsx::Ellipsoid elli_recip = pk->qShape();
        nsx::Intensity intensity = pk->rawIntensity();

        std::cout << "real peak: " << elli_real.center().transpose() << ", ";
        std::cout << "recip peak: " << elli_recip.center().transpose() << ", ";
        std::cout << "intensity: " << intensity.value() << " +- " << intensity.sigma() << std::endl;
    }
#endif
    std::cout << "--------------------------------------------------------------------------------\n" << std::endl;




    std::cout << "\n--------------------------------------------------------------------------------" << std::endl;
    std::cout << "Integrating peaks..." << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    nsx::IPeakIntegrator* integrator = exp.getIntegrator(std::string("Pixel sum integrator"));
    integrator->setPeakEnd(3.0);
    integrator->setBkgBegin(3.5);
    integrator->setBkgEnd(4.5);
    integrator->setHandler(progressHandler);
    exp.integrateFoundPeaks("Pixel sum integrator");
    exp.acceptFoundPeaks("found_peaks");
    nsx::PeakCollection* found_collection = exp.getPeakCollection("found_peaks");

    for(int i=0; i<found_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = found_collection->getPeak(i);
        double d = 1.0 / peak_ptr->q().rowVector().norm();

        std::cout << "Found peak " << i << ": d = " << d << std::endl;
    }

    std::cout << "--------------------------------------------------------------------------------\n" << std::endl;




    std::cout << "\n--------------------------------------------------------------------------------" << std::endl;
    std::cout << "Filtering peaks..." << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    nsx::PeakFilter* peak_filter = exp.peakFilter();
    peak_filter->setFilterDRange(true);
    const std::array<double, 2> d_range {0.5, 10.0};
    peak_filter->setDRange(d_range);

    peak_filter->resetFiltering(found_collection);
    peak_filter->filter(found_collection);

    exp.acceptFilter("filtered_peaks", found_collection);

    auto filtered_collection = exp.getPeakCollection("filtered_peaks");
    auto filteredPeaks = filtered_collection->getPeakList();

    for (const nsx::Peak3D* pk : filteredPeaks) {
        if (!pk->enabled())
            continue;

        nsx::Ellipsoid elli_real = pk->shape();
        nsx::Ellipsoid elli_recip = pk->qShape();
        nsx::Intensity intensity = pk->rawIntensity();
        nsx::Intensity background = pk->meanBackground();

        std::cout << "real peak: " << elli_real.center().transpose() << ", ";
        std::cout << "recip peak: " << elli_recip.center().transpose() << ", ";
        std::cout << "intensity: " << intensity.value() << "+-" << intensity.sigma() << ", ";
        std::cout << "background: " << background.value() << "+-" << background.sigma()
                  << std::endl;
    }


    for(int i=0; i<filtered_collection->numberOfPeaks(); ++i) {
        nsx::Peak3D* peak_ptr = filtered_collection->getPeak(i);
        double d = 1.0 / peak_ptr->q().rowVector().norm();

        std::cout << "Filtered peak " << i << ": d = " << d << std::endl;
    }
    std::cout << "--------------------------------------------------------------------------------\n" << std::endl;





    std::cout << "\n--------------------------------------------------------------------------------" << std::endl;
    std::cout << "Indexing peaks..." << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    nsx::AutoIndexer* auto_indexer = exp.autoIndexer();
    nsx::IndexerParameters parameters;
    std::cout << "AutoIndexer parameters: ";
    std::cout << "maxdim = " << parameters.maxdim << ", ";
    std::cout << "nSolutions = " << parameters.nSolutions << ", ";
    std::cout << "nVertices = " << parameters.nVertices << ", ";
    std::cout << "subdiv = " << parameters.subdiv << ", ";
    std::cout << "indexingTolerance = " << parameters.indexingTolerance << ", ";
    std::cout << "niggliTolerance = " << parameters.niggliTolerance << ", ";
    std::cout << "gruberTolerance = " << parameters.gruberTolerance << ", ";
    std::cout << "niggliReduction = " << parameters.niggliReduction << ", ";
    std::cout << "minUnitCellVolume = " << parameters.minUnitCellVolume << ", ";
    std::cout << "unitCellEquivalenceTolerance = " << parameters.unitCellEquivalenceTolerance << ", ";
    std::cout << "solutionCutoff = " << parameters.solutionCutoff << std::endl;

    //parameters.minUnitCellVolume = 1.;
    //parameters.maxdim = 500.;
    //parameters.nVertices = 1000;
    //parameters.solutionCutoff = 5.;
    auto_indexer->setParameters(parameters);

    auto peaksToIndex = filtered_collection->getPeakList();
    CHECK_NOTHROW(auto_indexer->autoIndex(peaksToIndex));
    CHECK(auto_indexer->solutions().size() > 1);

    std::cout << "Number of solutions: " << auto_indexer->solutions().size() << std::endl;
    for (std::size_t solidx = 0; solidx < auto_indexer->solutions().size(); ++solidx) {
        const auto& solution = auto_indexer->solutions()[solidx];
        const auto& cell = solution.first;
        double prob = solution.second;

        std::cout
            << "\n================================================================================"
            << std::endl;
        std::cout << "Solution " << solidx << " has " << prob << "% probability." << std::endl;
        std::cout << "Unit cell:" << std::endl;
        cell->printSelf(std::cout);
        std::cout
            << "================================================================================\n"
            << std::endl;
    }

    auto solution = auto_indexer->solutions().front();
    std::cout << "--------------------------------------------------------------------------------\n" << std::endl;

}
