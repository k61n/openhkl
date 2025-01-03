//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestIntegration.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/algo/Refiner.h"
#include "core/convolve/AnnularConvolver.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/Integrator.h"
#include "core/experiment/PeakFinder.h"
#include "core/experiment/ShapeModelBuilder.h"
#include "core/instrument/Diffractometer.h"
#include "core/integration/IIntegrator.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeModel.h"
#include "core/statistics/PeakMerger.h"
#include "tables/crystal/UnitCell.h"
#include "test/cpp/catch.hpp"


#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

TEST_CASE("test/data/TestShapeAssignment.cpp", "")
{
    // reference values
    const int ref_n_peaks = 59086;
    const int ref_n_integrated_peaks = 56186;
    const double ref_rpim_overall = 0.0774;
    const double ref_ccstar_overall = 0.9975;
    const std::vector<double> ref_rpim =
        {0.0274, 0.0605,  0.1028, 0.1313, 0.1731, 0.2189, 0.2592, 0.3008, 0.3742, 0.4729};
    const std::vector<double> ref_ccstar =
        {0.9986, 0.9955, 0.9875, 0.9802, 0.9730, 0.9514, 0.9394, 0.9285, 0.8794, 0.8053};

    const int eps_peaks = 100;
    const double eps_stats = 0.02;

    const std::string filename = "Trypsin.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    const ohkl::sptrUnitCell cell = experiment.getSptrUnitCell("indexed");
    const ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* found_peaks = experiment.getPeakCollection("found");

    data->initBuffer(true); // Required for parallel integration (otherwise HDF5 errors)

    // Construct shape model
    std::cout << "Constructing shape model" << std::endl;
    ohkl::ShapeModelBuilder* shape_builder = experiment.shapeModelBuilder();
    ohkl::ShapeModelParameters* shape_params = shape_builder->parameters();
    found_peaks->computeSigmas();
    shape_params->sigma_d = found_peaks->sigmaD();
    shape_params->sigma_m = found_peaks->sigmaM();
    shape_params->strength_min = 1.0;
    shape_params->interpolation = ohkl::PeakInterpolation::InverseDistance;
    shape_params->nbins_x = 20;
    shape_params->nbins_y = 20;
    shape_params->nbins_z = 6;
    shape_params->neighbour_range_pixels = 100;
    shape_params->neighbour_range_frames = 20;
    shape_params->region_type = ohkl::RegionType::FixedEllipsoid;
    shape_params->fixed_peak_end = 5.5;
    shape_params->fixed_bkg_begin = 1.3;
    shape_params->fixed_bkg_end = 2.3;
    ohkl::ShapeModel shapes = shape_builder->build(found_peaks, data);

    // Predict
    std::cout << "Predicting peaks" << std::endl;
    ohkl::Predictor* predictor = experiment.predictor();
    auto* predictor_params = predictor->parameters();
    predictor_params->d_min = 1.5;
    predictor_params->d_max = 50.0;
    predictor->predictPeaks(data, cell);
    const std::string predicted_peaks_name = "predicted_new";
    experiment.addPeakCollection(
        predicted_peaks_name, ohkl::PeakCollectionType::PREDICTED, predictor->peaks(), data, cell);
    ohkl::PeakCollection* predicted_peaks = experiment.getPeakCollection(predicted_peaks_name);
    shapes.setPredictedShapes(predicted_peaks);

    // Refine
    std::cout << "Refining" << std::endl;
    ohkl::Refiner* refiner = experiment.refiner();
    auto* refiner_params = refiner->parameters();
    refiner_params->nbatches = 30;
    refiner_params->refine_ki = true;
    refiner_params->refine_ub = true;
    refiner_params->refine_sample_orientation = true;
    refiner_params->refine_sample_position = true;
    refiner_params->refine_detector_offset = true;
    static_cast<void>(refiner->refine(data, found_peaks->getPeakList(), cell));
    static_cast<void>(refiner->updatePredictions(predicted_peaks->getPeakList()));

    std::cout << "Integrating" << std::endl;
    auto* integrator = experiment.integrator();
    auto* params = integrator->parameters();

    params->integrator_type = ohkl::IntegratorType::PixelSum;
    params->region_type = ohkl::RegionType::FixedEllipsoid;
    params->fixed_peak_end = 5.5;
    params->fixed_bkg_begin = 1.3;
    params->fixed_bkg_end = 2.3;
    params->max_counts = 50000.0;
    params->fit_center = true;
    params->fit_cov = true;
    params->discard_saturated = true;
    params->use_gradient = true;
    params->gradient_type = ohkl::GradientKernel::Sobel;
    params->fft_gradient = false;
    params->skip_masked = true;
    params->use_max_width = true;
    params->max_width = 10;

    data->initBuffer(true); // buffer all images (required for parallel integration)
    integrator->integratePeaks(data, predicted_peaks, params, nullptr);

    const int n_peaks = integrator->numberOfPeaks();
    const int n_integrated_peaks = integrator->numberOfValidPeaks();

    CHECK(n_peaks >= ref_n_peaks - eps_peaks);
    CHECK(n_integrated_peaks >= ref_n_integrated_peaks - eps_peaks);

    std::cout << "Merging" << std::endl;
    auto* merger = experiment.peakMerger();
    auto* merge_params = merger->parameters();

    merge_params->d_min = 1.5;
    merge_params->d_max = 50.0;
    merge_params->n_shells = 10;
    merge_params->friedel = true;

    const ohkl::SpaceGroup space_group = cell->spaceGroup();
    merger->setSpaceGroup(space_group);
    merger->addPeakCollection(predicted_peaks);
    merger->mergePeaks();
    merger->computeQuality();
    std::cout << merger->summary() << std::endl;

    auto* overall_quality = merger->sumOverallQuality();
    auto* shell_quality = merger->sumShellQuality();
    const double rpim_overall = overall_quality->shells[0].Rpim;
    const double ccstar_overall = overall_quality->shells[0].CCstar;
    std::vector<double> rpim;
    std::vector<double> ccstar;
    for (int i = 0; i < merge_params->n_shells; ++i) {
        rpim.push_back(shell_quality->shells[i].Rpim);
        ccstar.push_back(shell_quality->shells[i].CCstar);
    }

    CHECK(rpim_overall < ref_rpim_overall + eps_stats);
    CHECK(ccstar_overall < ref_ccstar_overall + eps_stats);
    for (int i = 0; i < rpim.size(); ++i) {
        CHECK(rpim[i] < ref_rpim[i] + eps_stats);
        CHECK(ccstar[i] < ref_ccstar[i] + eps_stats);
        std::cout << rpim[i] << " " << ref_rpim[i] << std::endl;
        std::cout << ccstar[i] << " " << ref_ccstar[i] << std::endl;
    }
}
