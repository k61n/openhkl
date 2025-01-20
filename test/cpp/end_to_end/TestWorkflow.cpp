//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestWorkflow.cpp
//! @brief     Test full workflow, small (20 image) data set
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/Integrator.h"
#include "core/experiment/PeakFinder.h"
#include "core/experiment/ShapeModelBuilder.h"
#include "core/image/ImageFilter.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/integration/IIntegrator.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeModel.h"
#include "core/statistics/PeakMerger.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"
#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <regex>

TEST_CASE("test/data/TestWorkflow.cpp", "")
{

    // names
    const std::string expt_name = "Trypsin";
    const std::string diff_name = "BioDiff";
    const std::string data_name = "Scan I";
    const std::string found_peaks_name = "found";
    const std::string predicted_peaks_name = "predicted";
    const std::string found_unit_cell_name = "indexed";

    // Reference data
    const int ref_found_peaks = 1777;
    const int ref_found_integrated_peaks = 1489;
    const int ref_shapes = 1513;
    const int ref_predicted_peaks = 6239;
    const int ref_valid_predicted_peaks = 6237;
    const int ref_updated = 5487;
    const int ref_integrated = 5726;
    const std::vector<double> ref_rmerge = {
        0.0573, 0.1416, 0.2196, 0.2523, 0.2863, 0.3207, 0.3967, 0.6299, 0.5787, 0.8015};
    const std::vector<double> ref_cchalf = {
        0.9851, 0.9324, 0.5589, 0.7498, 0.6945, 0.6711, 0.5623, 0.0520, 0.3960, -0.1157};
    const std::vector<double> ref_completeness = {
        0.2196, 0.1952, 0.1461, 0.1376, 0.1323, 0.1216, 0.1178, 0.1148, 0.1108, 0.0870};

    // Numerical check thresholds
    const int eps_peaks = 10;
    const double eps_stat = 0.01;

    // Read the files
    const std::string path = ".";
    const std::regex regexpr{"p11202_.*?tiff"};
    const int nfiles = 20;
    std::vector<std::string> filenames;

    for (auto& file : std::filesystem::directory_iterator(path)) {
        if (std::regex_search(file.path().string(), regexpr))
            filenames.push_back(file.path().string());
    }
    std::sort(filenames.begin(), filenames.end());
    CHECK(filenames.size() == nfiles);

    ohkl::Experiment experiment(expt_name, diff_name);

    // Load images as DataSet
    std::cout << "Loading images" << std::endl;
    const ohkl::sptrDataSet new_data = std::make_shared<ohkl::DataSet>(
        data_name, experiment.getDiffractometer());
    ohkl::DataReaderParameters data_params;
    data_params.wavelength = 2.67;
    data_params.delta_omega = 0.4;
    data_params.data_format = ohkl::DataFormat::TIFF;
    data_params.rebin_size = 2;
    new_data->setImageReaderParameters(data_params);
    for (const auto& file : filenames)
        new_data->addFrame(file, ohkl::DataFormat::TIFF);
    new_data->finishRead();
    experiment.addData(new_data);
    ohkl::sptrDataSet data = experiment.getData(data_name);
    data->initBuffer(true);
    std::cout << "Loaded " << data->nFrames() << " images" << std::endl;
    CHECK(data->nFrames() == nfiles);

    // Add masks
    std::cout << "Adding masks" << std::endl;
    ohkl::AABB box_left = {{0, 0, 0}, {250, 900, 169}};
    ohkl::AABB box_right = {{2250, 0, 0}, {2500, 900, 169}};
    ohkl::AABB box_seam = {{1725, 0, 0}, {1740, 900, 169}};
    ohkl::AABB ellipse = {{1200, 400, 0}, {1300, 500, 169}};
    data->addMask(new ohkl::BoxMask(box_left));
    data->addMask(new ohkl::BoxMask(box_right));
    data->addMask(new ohkl::BoxMask(box_seam));
    data->addMask(new ohkl::EllipseMask(ellipse));
    std::cout << "Loaded " << data->getNMasks() << " masks" << std::endl;
    CHECK(data->getNMasks() == 4);

    // Find peaks
    std::cout << "Finding peaks" << std::endl;
    ohkl::PeakFinder* finder = experiment.peakFinder();
    auto* finder_params = finder->parameters();
    finder_params->first_frame = 0;
    finder_params->last_frame = -1;
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->peak_end = 1.0;
    finder_params->r1 = 5.0;
    finder_params->r2 = 10.0;
    finder_params->r3 = 15.0;
    finder_params->threshold = 1.1;
    finder_params->filter = "Enhanced annular";
    finder->find(data);
    CHECK(finder->numberFound() <= ref_found_peaks + eps_peaks);
    CHECK(finder->numberFound() >= ref_found_peaks - eps_peaks);
    std::cout << "Found " << finder->numberFound() << " peaks" << std::endl;

    // Integrate found peaks
    std::cout << "Integrating found peaks" << std::endl;
    ohkl::Integrator* integrator = experiment.integrator();
    auto* integrator_params = integrator->parameters();
    integrator_params->region_type = ohkl::RegionType::FixedEllipsoid;
    integrator_params->fixed_peak_end = 5.5;
    integrator_params->fixed_bkg_begin = 1.3;
    integrator_params->fixed_bkg_end = 2.3;
    integrator_params->use_gradient = false;
    integrator->integrateFoundPeaks(finder);
    experiment.acceptFoundPeaks(found_peaks_name);
    ohkl::PeakCollection* found_peaks = experiment.getPeakCollection(found_peaks_name);
    ohkl::RejectionFlag most_frequent = found_peaks->mostFrequentRejection();
    std::cout << "Most frequent rejection flag = " << static_cast<int>(most_frequent) << std::endl;
    CHECK(found_peaks->numberOfValid() <= ref_found_integrated_peaks + eps_peaks);
    CHECK(found_peaks->numberOfValid() >= ref_found_integrated_peaks - eps_peaks);
    std::cout << "Successfully integrated " << found_peaks->numberOfValid() << " peaks"
              << std::endl;

    // Set the reference unit cell
    ohkl::UnitCell reference_cell(
        54.7, 58.6, 67.3, 90 * ohkl::deg, 90 * ohkl::deg, 90 * ohkl::deg, data);
    ohkl::SpaceGroup group("P 21 21 21");
    reference_cell.setSpaceGroup(group);

    // Autoindex
    std::cout << "Autoindexing found peaks" << std::endl;
    ohkl::AutoIndexer* indexer = experiment.autoIndexer();
    ohkl::IndexerParameters* indexer_params = indexer->parameters();

    indexer_params->d_min = 1.5;
    indexer_params->first_frame = 0;
    indexer_params->last_frame = 9;

    indexer->autoIndex(found_peaks, data);

    std::cout << indexer->solutionsToString() << std::endl;

    ohkl::sptrUnitCell good_cell = indexer->firstGoodSolution(
        54.7, 58.6, 67.3, 90, 90, 90, 1.0, 0.1, group.bravaisTypeSymbol());
    REQUIRE(good_cell != nullptr);
    good_cell->setSpaceGroup(group);
    CHECK(good_cell); // nullptr if none found
    experiment.addUnitCell(found_unit_cell_name, *good_cell);
    ohkl::sptrUnitCell cell = experiment.getSptrUnitCell(found_unit_cell_name);
    experiment.assignUnitCell(found_peaks, found_unit_cell_name);
    found_peaks->setMillerIndices();
    std::cout << "Reference cell: " << reference_cell.toString() << std::endl;
    std::cout << "Indexed cell:   " << cell->toString() << std::endl;
    CHECK(cell->isSimilar(&reference_cell, 1.0, 0.1));

    // Construct shape model
    std::cout << "Constructing shape model" << std::endl;
    auto* shape_builder = experiment.shapeModelBuilder();
    auto* shape_params = shape_builder->parameters();

    found_peaks->computeSigmas();
    shape_params->sigma_d = found_peaks->sigmaD();
    shape_params->sigma_m = found_peaks->sigmaM();
    shape_params->neighbour_range_pixels = 250;
    shape_params->neighbour_range_frames = 10;
    shape_params->n_subdiv = 5;
    shape_params->region_type = ohkl::RegionType::FixedEllipsoid;
    shape_params->fixed_peak_end = 5.5;
    shape_params->fixed_bkg_begin = 1.3;
    shape_params->fixed_bkg_end = 2.3;
    shape_params->strength_min = 5.0;
    shape_params->d_min = 1.5;

    ohkl::ShapeModel shapes = shape_builder->integrate(found_peaks->getPeakList(), data);
    std::cout << "Shape model contains " << shapes.numberOfPeaks() << " profiles" << std::endl;
    CHECK(shapes.numberOfPeaks() <= ref_shapes + eps_peaks);
    CHECK(shapes.numberOfPeaks() >= ref_shapes - eps_peaks);

    // Predict
    std::cout << "Predicting peaks" << std::endl;
    ohkl::Predictor* predictor = experiment.predictor();
    auto* predictor_params = predictor->parameters();
    predictor_params->d_min = 1.5;
    predictor_params->d_max = 50.0;
    predictor->predictPeaks(data, cell);
    std::cout << predictor->numberOfPredictedPeaks() << " peaks predicted" << std::endl;
    CHECK(predictor->numberOfPredictedPeaks() >= ref_predicted_peaks - eps_peaks);
    CHECK(predictor->numberOfPredictedPeaks() <= ref_predicted_peaks + eps_peaks);
    experiment.addPeakCollection(
        predicted_peaks_name, ohkl::PeakCollectionType::PREDICTED, predictor->peaks(), data, cell);
    ohkl::PeakCollection* predicted_peaks = experiment.getPeakCollection(predicted_peaks_name);
    shapes.setPredictedShapes(predicted_peaks);
    std::cout << predicted_peaks->numberOfValid() << " valid predicted peaks" << std::endl;
    CHECK(predicted_peaks->numberOfValid() <= ref_valid_predicted_peaks + eps_peaks);
    CHECK(predicted_peaks->numberOfValid() >= ref_valid_predicted_peaks - eps_peaks);

    // Refine
    std::cout << "Refining" << std::endl;
    ohkl::Refiner* refiner = experiment.refiner();
    auto* refiner_params = refiner->parameters();
    refiner_params->nbatches = 5;
    bool refine_success = refiner->refine(data, found_peaks->getPeakList(), cell);
    CHECK(refine_success);
    int n_updated = refiner->updatePredictions(predicted_peaks->getPeakList());
    CHECK(n_updated == ref_updated);
    std::cout << n_updated << " peaks updated" << std::endl;

    // Integrate predictions
    integrator_params->integrator_type = ohkl::IntegratorType::PixelSum;
    integrator_params->region_type = ohkl::RegionType::FixedEllipsoid;
    integrator_params->peak_end = 5.5;
    integrator_params->bkg_begin = 1.3;
    integrator_params->bkg_end = 2.3;
    integrator_params->use_gradient = false;
    integrator->integratePeaks(data, predicted_peaks, integrator_params, &shapes);
    CHECK(integrator->numberOfValidPeaks() <= ref_integrated + eps_peaks);
    CHECK(integrator->numberOfValidPeaks() >= ref_integrated - eps_peaks);
    std::cout << integrator->numberOfValidPeaks() << " / " << integrator->numberOfPeaks()
              << " integrated" << std::endl;

    // Merge predicted peaks
    std::cout << "Merging" << std::endl;
    ohkl::PeakMerger* merger = experiment.peakMerger();
    auto* merger_params = merger->parameters();
    merger_params->d_min = 1.5;
    merger_params->d_max = 50.0;
    merger->addPeakCollection(predicted_peaks);
    merger->setSpaceGroup(group);
    merger->mergePeaks();
    merger->computeQuality();
    std::cout << merger->summary() << std::endl;

    for (std::size_t idx = 0; idx < 7; ++idx) {
        CHECK_THAT(
            merger->sumShellQuality()->shells.at(idx).Rmerge,
            Catch::Matchers::WithinAbs(ref_rmerge.at(idx), eps_stat));
        // CHECK_THAT(
        //     merger->sumShellQuality()->shells.at(idx).CChalf,
        //     Catch::Matchers::WithinAbs(ref_cchalf.at(idx), eps_stat));
        CHECK_THAT(
            merger->sumShellQuality()->shells.at(idx).Completeness,
            Catch::Matchers::WithinAbs(ref_completeness.at(idx), eps_stat));
    }
    // experiment.saveToFile("Trypsin-small.ohkl"); // Regenerate the end-to-end test file
}

