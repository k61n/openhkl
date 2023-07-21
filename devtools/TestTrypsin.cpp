//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestTrypsin.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "core/algo/AutoIndexer.h"
#include "core/algo/Refiner.h"
#include "core/convolve/AnnularConvolver.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/integration/IIntegrator.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/IntegrationRegion.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeModel.h"
#include "core/statistics/PeakMerger.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"
#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <regex>

TEST_CASE("test/data/TestTrypsin.cpp", "")
{
    auto isSimilar = [](int value, int ref, int eps){
        std::cout << value << " " << ref << std::endl;
        return ( value < (ref + eps) && value > (ref - eps));
    };

    // names
    const std::string expt_name = "Trypsin";
    const std::string diff_name = "BioDiff";
    const std::string data_name = "Scan I";
    const std::string found_peaks_name = "found";
    const std::string predicted_peaks_name = "predicted";
    const std::string found_unit_cell_name = "indexed";

    // Reference data
    const int ref_found_peaks = 25377;
    const int ref_found_integrated_peaks = 16640;
    const int ref_shapes = 16613;
    const int ref_predicted_peaks = 58247;
    const int ref_valid_predicted_peaks = 58215;
    const int ref_updated = 56750;
    const int ref_integrated = 54539;
    const std::vector<double> ref_rmerge = {
        0.0424, 0.0911, 0.1508, 0.1999, 0.2456, 0.2974, 0.3372, 0.3820, 0.4571, 0.5739};
    const std::vector<double> ref_completeness = {
        0.9529, 0.8976, 0.7310, 0.7268, 0.7084, 0.6830, 0.6675, 0.6572, 0.6400, 0.5395};

    // Numerical check thresholds
    const int eps_peaks = 10;
    const double eps_stat = 0.01;

    // Read the files
    const std::string path = ".";
    const std::regex regexpr{"soak_9_d2.*?raw"};
    int nfiles = 169;
    std::vector<std::string> filenames;

    for (auto& file : std::filesystem::directory_iterator(path)) {
        if (std::regex_search(file.path().string(), regexpr))
            filenames.push_back(file.path().string());
    }
    CHECK(filenames.size() == nfiles);

    ohkl::Experiment experiment(expt_name, diff_name);

    // Load images as DataSet
    std::cout << "Loading images" << std::endl;
    const ohkl::sptrDataSet new_data = std::make_shared<ohkl::DataSet>(
        data_name, experiment.getDiffractometer());
    ohkl::DataReaderParameters data_params;
    data_params.wavelength = 2.67;
    data_params.delta_omega = 0.4;
    new_data->setRawReaderParameters(data_params);
    for (const auto& file : filenames)
        new_data->addRawFrame(file);
    new_data->finishRead();
    experiment.addData(new_data);
    ohkl::sptrDataSet data = experiment.getData(data_name);
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
    finder_params->frames_begin = 0;
    finder_params->frames_end = -1;
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->peak_end = 1.0;
    finder_params->threshold = 30;
    finder->setConvolver(ohkl::AnnularConvolver());
    finder->find(data);
    CHECK(isSimilar(finder->numberFound(), ref_found_peaks, eps_peaks));
    std::cout << "Found " << finder->numberFound() << " peaks" << std::endl;

    // Integrate found peaks
    std::cout << "Integrating found peaks" << std::endl;
    ohkl::Integrator* integrator = experiment.integrator();
    auto* integrator_params = integrator->parameters();
    integrator_params->peak_end = 3.0;
    integrator_params->bkg_begin = 3.0;
    integrator_params->bkg_end = 6.0;
    integrator->integrateFoundPeaks(finder);
    experiment.acceptFoundPeaks(found_peaks_name);
    ohkl::PeakCollection* found_peaks = experiment.getPeakCollection(found_peaks_name);
    CHECK(isSimilar(found_peaks->numberOfValid(), ref_found_integrated_peaks, eps_peaks));
    std::cout << "Successfully integrated " << found_peaks->numberOfValid() << " peaks"
              << std::endl;

    // Set the reference unit cell
    experiment.setReferenceCell(54.9, 58.4, 67.3, 90, 90, 90, data);
    ohkl::SpaceGroup group{"P 21 21 21"};
    ohkl::sptrUnitCell ref_cell = experiment.getSptrUnitCell("reference");
    ref_cell->setSpaceGroup(group);

    // Autoindex
    std::cout << "Autoindexing found peaks" << std::endl;
    data->adjustDirectBeam(-1.0, -2.0);
    ohkl::AutoIndexer* indexer = experiment.autoIndexer();
    indexer->autoIndex(found_peaks);
    ohkl::sptrUnitCell good_cell = indexer->goodSolution(ref_cell.get(), 1.0, 0.1);
    good_cell->setSpaceGroup(group);
    CHECK(good_cell); // nullptr if none found
    experiment.addUnitCell(found_unit_cell_name, *good_cell);
    ohkl::sptrUnitCell cell = experiment.getSptrUnitCell(found_unit_cell_name);
    experiment.assignUnitCell(found_peaks, found_unit_cell_name);
    found_peaks->setMillerIndices();
    std::cout << "Reference cell: " << ref_cell->toString() << std::endl;
    std::cout << "Indexed cell:   " << cell->toString() << std::endl;
    CHECK(cell->isSimilar(ref_cell.get(), 1.0, 0.1));

    // Construct shape model
    std::cout << "Constructing shape model" << std::endl;
    ohkl::ShapeModelParameters shape_params;
    found_peaks->computeSigmas();
    shape_params.sigma_d = found_peaks->sigmaD();
    shape_params.sigma_m = found_peaks->sigmaM();
    shape_params.strength_min = 3.0;
    found_peaks->buildShapeModel(data, shape_params);
    ohkl::ShapeModel* shapes = found_peaks->shapeModel();
    std::cout << "Shape model contains " << shapes->numberOfPeaks() << " profiles" << std::endl;
    CHECK(isSimilar(shapes->numberOfPeaks(), ref_shapes, eps_peaks));

    // Predict
    std::cout << "Predicting peaks" << std::endl;
    ohkl::Predictor* predictor = experiment.predictor();
    auto* predictor_params = predictor->parameters();
    predictor_params->d_min = 1.5;
    predictor_params->d_max = 50.0;
    predictor->predictPeaks(data, cell);
    std::cout << predictor->numberOfPredictedPeaks() << " peaks predicted" << std::endl;
    CHECK(isSimilar(predictor->numberOfPredictedPeaks(), ref_predicted_peaks, eps_peaks));
    experiment.addPeakCollection(
        predicted_peaks_name, ohkl::PeakCollectionType::PREDICTED, predictor->peaks(), data, cell);
    ohkl::PeakCollection* predicted_peaks = experiment.getPeakCollection(predicted_peaks_name);
    found_peaks->shapeModel()->setPredictedShapes(predicted_peaks);
    std::cout << predicted_peaks->numberOfValid() << " valid predicted peaks" << std::endl;
    CHECK(isSimilar(predicted_peaks->numberOfValid(), ref_valid_predicted_peaks, eps_peaks));

    // Refine
    std::cout << "Refining" << std::endl;
    ohkl::Refiner* refiner = experiment.refiner();
    auto* refiner_params = refiner->parameters();
    refiner_params->nbatches = 30;
    refiner->makeBatches(data->instrumentStates(), found_peaks->getPeakList(), cell);
    bool refine_success = refiner->refine();
    CHECK(refine_success);
    int n_updated = refiner->updatePredictions(predicted_peaks->getPeakList());
    std::cout << n_updated << " peaks updated" << std::endl;
    CHECK(isSimilar(n_updated, ref_updated, eps_peaks));

    // Integrate predictions
    integrator_params->integrator_type = ohkl::IntegratorType::PixelSum;
    integrator_params->region_type = ohkl::RegionType::FixedEllipsoid;
    integrator_params->peak_end = 5.5;
    integrator_params->bkg_begin = 1.3;
    integrator_params->bkg_end = 2.3;
    integrator->integratePeaks(data, predicted_peaks, integrator_params, found_peaks->shapeModel());
    std::cout << integrator->numberOfValidPeaks() << " / " << integrator->numberOfPeaks()
              << " integrated" << std::endl;
    CHECK(isSimilar(integrator->numberOfValidPeaks(), ref_integrated, eps_peaks));

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

    for (std::size_t idx = 0; idx < merger_params->n_shells; ++idx) {
        CHECK(merger->shellQuality()->shells.at(idx).Rmerge < ref_rmerge.at(idx) + eps_stat);
        CHECK(
            merger->shellQuality()->shells.at(idx).Completeness > ref_completeness.at(idx)
            - eps_stat);
    }
}

