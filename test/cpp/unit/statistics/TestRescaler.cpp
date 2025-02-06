//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestRescaler.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/Rescaler.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/src/Core/Matrix.h>
#include <iostream>

TEST_CASE("test/integrate/TestRescaler.cpp", "")
{
    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");
    ohkl::UnitCell* cell = experiment.getUnitCell("indexed");

    std::vector<ohkl::PeakCollection*> peak_collections = {peaks};
    ohkl::MergedPeakCollection merged_peaks(cell->spaceGroup(), peak_collections, true, true);
    merged_peaks.setDRange(1.5, 50.0);

    ohkl::Rescaler rescaler(data, &merged_peaks);
    rescaler.optimize(1000);
    const Eigen::VectorXd& scale_factors = rescaler.scaleFactors();

    for (std::size_t idx = 0; idx < data->nFrames(); ++idx) {
        std::cout << scale_factors(idx) << std::endl;
    }


    CHECK(false);
    // CHECK_THAT(merged_peaks.redundancy(), Catch::Matchers::WithinAbs(ref_redundancy, eps));
}
