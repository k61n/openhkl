//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestRFactor.cpp
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
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/RFactor.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include <iostream>

TEST_CASE("test/integrate/TestRFactor.cpp", "")
{
    const double eps = 1.0e-5;

    const double ref_rmerge = 0.1569480903;
    const double ref_rmeas = 0.2165108297;
    const double ref_rpim = 0.1485273123;
    const double ref_expected_rmerge = 0.006849476;
    const double ref_expected_rmeas = 0.0095463333;
    const double ref_expected_rpim = 0.0066326086;

    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");
    ohkl::UnitCell* cell = experiment.getUnitCell("indexed");

    std::vector<ohkl::PeakCollection*> peak_collections = {peaks};
    ohkl::MergedPeakCollection merged_peaks(cell->spaceGroup(), peak_collections, true, true);

    ohkl::RFactor rfactor(true);
    rfactor.calculate(&merged_peaks);


    CHECK_THAT(rfactor.Rmerge(), Catch::Matchers::WithinAbs(ref_rmerge, eps));
    CHECK_THAT(rfactor.Rmeas(), Catch::Matchers::WithinAbs(ref_rmeas, eps));
    CHECK_THAT(rfactor.Rpim(), Catch::Matchers::WithinAbs(ref_rpim, eps));
    CHECK_THAT(rfactor.expectedRmerge(), Catch::Matchers::WithinAbs(ref_expected_rmerge, eps));
    CHECK_THAT(rfactor.expectedRmeas(), Catch::Matchers::WithinAbs(ref_expected_rmeas, eps));
    CHECK_THAT(rfactor.expectedRpim(), Catch::Matchers::WithinAbs(ref_expected_rpim, eps));
}
