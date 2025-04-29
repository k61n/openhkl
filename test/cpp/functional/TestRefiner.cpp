//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestRefiner.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/shape/Predictor.h"
#include "tables/crystal/UnitCell.h"


#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

TEST_CASE("test/data/TestRefiner.cpp", "")
{
    const double eps = 1.0e-4;

    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrUnitCell cell = experiment.getSptrUnitCell("indexed");
    ohkl::sptrDataSet data = experiment.getData("Scan I");

    double ref_a = 54.91202;
    double ref_b = 58.46015;
    double ref_c = 66.74780;

    auto* predictor = experiment.predictor();
    auto* params = predictor->parameters();
    params->d_min = 1.5;
    params->d_max = 50.0;
    predictor->predictPeaks(data, cell);

    std::cout << predictor->numberOfPredictedPeaks() << " peaks predicted" << std::endl;

    experiment.addPeakCollection(
        "predicted", ohkl::PeakCollectionType::PREDICTED, predictor->peaks(), data, cell);

    ohkl::PeakCollection* found_peaks = experiment.getPeakCollection("found");
    ohkl::PeakCollection* predicted_peaks = experiment.getPeakCollection("predicted");


    ohkl::Refiner* refiner = experiment.refiner();
    auto* refiner_params = refiner->parameters();
    refiner_params->refine_ub = true;
    refiner_params->refine_ki = false;
    refiner_params->refine_sample_position = false;
    refiner_params->refine_sample_orientation = false;
    refiner_params->refine_detector_offset = false;
    refiner_params->nbatches = 4;
    bool refine_success = refiner->refine(data, found_peaks->getPeakList(), cell);
    CHECK(refine_success);

    std::cout << "Original cell" << std::endl;
    std::cout << cell->toString() << std::endl;
    std::cout << "Refined cells" << std::endl;
    for (const auto& batch : refiner->batches()) {
        ohkl::UnitCell* cell = batch.cell();
        std::cout << cell->toString() << std::endl;
    }

    double diff_a_ref = 0.0371040485;
    double diff_b_ref = 0.0159894134;
    double diff_c_ref = 0.5872799761;
    double diff_a = std::fabs(refiner->batches()[3].cell()->character().a - ref_a);
    double diff_b = std::fabs(refiner->batches()[3].cell()->character().b - ref_b);
    double diff_c = std::fabs(refiner->batches()[3].cell()->character().c - ref_c);
    CHECK_THAT(diff_a, Catch::Matchers::WithinAbs(diff_a_ref, eps));
    CHECK_THAT(diff_b, Catch::Matchers::WithinAbs(diff_b_ref, eps));
    CHECK_THAT(diff_c, Catch::Matchers::WithinAbs(diff_c_ref, eps));

    int n_updated = refiner->updatePredictions(predicted_peaks->getPeakList());
    std::cout << n_updated << " peaks updated" << std::endl;
    CHECK(n_updated == 5346);
}
