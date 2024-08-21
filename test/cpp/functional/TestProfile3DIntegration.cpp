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

#include "core/convolve/AnnularConvolver.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/Integrator.h"
#include "core/experiment/PeakFinder.h"
#include "core/experiment/ShapeModelBuilder.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/ShapeModel.h"
#include "core/statistics/PeakMerger.h"
#include "tables/crystal/UnitCell.h"
#include "test/cpp/catch.hpp"


#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

TEST_CASE("test/data/TestProfile3DIntegration.cpp", "")
{
    // reference values
    const int ref_n_peaks = 59209;
    const int ref_n_integrated_peaks = 55986;
    const double ref_rpim_overall = 0.0763;
    const double ref_ccstar_overall = 0.9973;
    const double ref_cchalf_overall = 0.9892;
    const std::vector<double> ref_rpim =
        {0.0267, 0.0605,  0.1019, 0.1325, 0.1729, 0.2113, 0.2474, 0.2790, 0.3272, 0.3974};
    const std::vector<double> ref_ccstar =
        {0.9988, 0.9956, 0.9873, 0.9786, 0.9685, 0.9464, 0.9330, 0.9283, 0.8837, 0.8071};
    const std::vector<double> ref_cchalf =
        {0.9953, 0.9826, 0.9507, 0.9187, 0.8833, 0.8114, 0.7707, 0.7570, 0.6406, 0.4831};

    const int eps_peaks = 100;
    const double eps_stats = 0.01;

    const std::string filename = "Trypsin-pxsum.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    const ohkl::sptrUnitCell cell = experiment.getSptrUnitCell("indexed");
    const ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");

    auto* integrator = experiment.integrator();
    auto* params = integrator->parameters();

    data->initBuffer(true); // buffer all images (required for parallel integration)

    ohkl::ShapeModelBuilder* shape_builder = experiment.shapeModelBuilder();
    ohkl::ShapeModelParameters* shape_params = shape_builder->parameters();
    ohkl::PeakCollection* found = experiment.getPeakCollection("found");
    found->computeSigmas();
    shape_params->sigma_d = found->sigmaD();
    shape_params->sigma_m = found->sigmaM();
    shape_params->strength_min = 1.0;
    shape_params->neighbour_range_pixels = 50;
    shape_params->neighbour_range_frames = 10;
    ohkl::ShapeModel shapes = shape_builder->build(found, data);

    params->integrator_type = ohkl::IntegratorType::Profile3D;
    params->region_type = ohkl::RegionType::FixedEllipsoid;
    params->fixed_peak_end = 5.5;
    params->fixed_bkg_begin = 1.3;
    params->fixed_bkg_end = 2.3;
    params->max_counts = 50000.0;
    params->discard_saturated = true;
    params->skip_masked = true;
    params->use_max_strength = true;
    params->max_strength = 1.0;
    params->use_max_d = true;
    params->max_d = 2.5;

    integrator->integratePeaks(data, peaks, params, &shapes);

    const int n_peaks = integrator->numberOfPeaks();
    const int n_integrated_peaks = integrator->numberOfValidPeaks();

    CHECK(n_peaks >= ref_n_peaks - eps_peaks);
    CHECK(n_integrated_peaks >= ref_n_integrated_peaks - eps_peaks);

    auto* merger = experiment.peakMerger();
    auto* merge_params = merger->parameters();

    merge_params->d_min = 1.5;
    merge_params->d_max = 50.0;
    merge_params->n_shells = 10;
    merge_params->friedel = true;

    const ohkl::SpaceGroup space_group = cell->spaceGroup();
    merger->setSpaceGroup(space_group);
    merger->addPeakCollection(peaks);
    merger->mergePeaks();
    merger->computeQuality();
    std::cout << merger->summary() << std::endl;

    auto* overall_quality = merger->profileOverallQuality();
    auto* shell_quality = merger->profileShellQuality();
    const double rpim_overall = overall_quality->shells[0].Rpim;
    const double ccstar_overall = overall_quality->shells[0].CCstar;
    const double cchalf_overall = overall_quality->shells[0].CChalf;
    std::vector<double> rpim;
    std::vector<double> ccstar;
    std::vector<double> cchalf;
    for (int i = 0; i < merge_params->n_shells; ++i) {
        rpim.push_back(shell_quality->shells[i].Rpim);
        ccstar.push_back(shell_quality->shells[i].CCstar);
        cchalf.push_back(shell_quality->shells[i].CChalf);
    }

    CHECK(rpim_overall < ref_rpim_overall + eps_stats);
    CHECK(ccstar_overall > ref_ccstar_overall - eps_stats);
    CHECK(cchalf_overall > ref_cchalf_overall - eps_stats);
    std::cout << "rpim_overall = " << rpim_overall << std::endl;
    std::cout << "ccstar_overall = " << ccstar_overall << std::endl;
    std::cout << "cchalf_overall = " << cchalf_overall << std::endl;
    for (int i = 0; i < rpim.size(); ++i) {
        CHECK(rpim[i] < ref_rpim[i] + eps_stats);
        CHECK(ccstar[i] > ref_ccstar[i] - eps_stats);
        CHECK(cchalf[i] > ref_cchalf[i] - eps_stats);
        std::cout << rpim[i] << " " << ref_rpim[i] << std::endl;
        std::cout << ccstar[i] << " " << ref_ccstar[i] << std::endl;
        std::cout << cchalf[i] << " " << ref_cchalf[i] << std::endl << std::endl;
    }
}
