//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/crystal/TestRefiner.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/algo/DataReaderFactory.h"
#include "core/algo/Refiner.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/experiment/UnitCellHandler.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/peak/Peak3D.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakFilter.h"

TEST_CASE("test/crystal/TestRefiner.cpp", "")
{
    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.getDiffractometer()));

    experiment.addData(dataf);

    nsx::sptrProgressHandler progressHandler(new nsx::ProgressHandler);

    auto callback = [progressHandler]() {
        auto log = progressHandler->getLog();
        for (const auto& msg : log)
            std::cout << msg << std::endl;
    };

    progressHandler->setCallback(callback);

    // #########################################################
    // test the finder
    nsx::DataList numors;
    numors.push_back(dataf);

    nsx::ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create("annular", {});

    nsx::PeakFinder* peak_finder = experiment.peakFinder();

    auto finder_params = peak_finder->parameters();
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->peak_end = 1.0;
    finder_params->maximum_frames = 10;
    finder_params->frames_begin = 0;
    finder_params->frames_end = dataf->nFrames();
    finder_params->threshold = 15;

    peak_finder->setConvolver(std::unique_ptr<nsx::Convolver>(convolver));
    peak_finder->setHandler(progressHandler);
    peak_finder->find(numors);

    auto found_peaks = peak_finder->currentPeaks();

    try {
        CHECK(static_cast<int>(found_peaks.size()) >= 0);
    } catch (...) {
        std::cout << "ERROR: exception in PeakFinder::find()" << std::endl;
    }

    CHECK(found_peaks.size() >= 800);

    nsx::IPeakIntegrator* integrator =
        experiment.getIntegrator(nsx::IntegratorType::PixelSum);
    nsx::IntegrationParameters params{};
    params.peak_end = 2.7;
    params.bkg_begin = 3.5;
    params.bkg_end = 4.0;
    integrator->setParameters(params);
    integrator->setHandler(progressHandler);
    experiment.integrateFoundPeaks(nsx::IntegratorType::PixelSum);
    experiment.acceptFoundPeaks("found_peaks");

    // #########################################################
    // Filter the peaks
    nsx::PeakFilter* peak_filter = experiment.peakFilter();
    peak_filter->setFilterDRange(true);
    peak_filter->parameters()->d_min = 1.5;
    peak_filter->parameters()->d_max = 50.0;

    nsx::PeakCollection* found_collection = experiment.getPeakCollection("found_peaks");
    peak_filter->resetFiltering(found_collection);
    peak_filter->filter(found_collection);

    experiment.acceptFilter("filtered_peaks", found_collection);

    CHECK(experiment.getPeakCollection("filtered_peaks")->getPeakList().size() >= 600);

    // #########################################################
    // at this stage we have the peaks, now we index
    nsx::AutoIndexer* auto_indexer = experiment.autoIndexer();
    nsx::PeakCollection* filtered_peaks = experiment.getPeakCollection("filtered_peaks");

    CHECK_NOTHROW(auto_indexer->autoIndex(filtered_peaks->getPeakList()));
    CHECK(auto_indexer->solutions().size() > 1);

    auto solution = auto_indexer->solutions().front();

    // correctly indexed at least 98% of peaks
    CHECK(solution.second > 98.0);


    // set unit cell
    const auto cell = solution.first;

    // set constraints
    auto constrained_cell = cell->applyNiggliConstraints();
    CHECK(std::abs((cell->reciprocalBasis() - constrained_cell.reciprocalBasis()).norm()) < 1e-6);

    std::vector<nsx::Peak3D*> peaks;
    for (const auto& peak : filtered_peaks->getPeakList()) {
        peak->setUnitCell(cell.get());
        peaks.push_back(peak);
    }

    auto&& states = dataf->instrumentStates();
    nsx::UnitCellHandler* cell_handler = experiment.getCellHandler();

    nsx::RefinerParameters refiner_params{};
    refiner_params.nbatches = 1;
    refiner_params.refine_ub = true;
    refiner_params.refine_sample_position = true;
    refiner_params.refine_detector_offset = false;
    refiner_params.refine_sample_orientation = false;
    refiner_params.refine_ki = false;
    nsx::Refiner refiner(states, cell.get(), peaks, refiner_params, cell_handler);

    CHECK(refiner.batches().size() == 1);

    for (const auto& batch : refiner.batches())
        CHECK(batch.peaks().size() > 200);

    std::cout << "peaks to refine: " << peaks.size() << std::endl;

    CHECK(refiner.refine(500));
}
