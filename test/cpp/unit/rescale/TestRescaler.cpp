//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestRescaler.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/Experiment.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/RFactor.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

#include "test/cpp/catch.hpp"

#include "core/rescale/Rescaler.h"

#include <iostream>


TEST_CASE("test/data/TestRescaler.cpp", "")
{
    const double ref_initial_rmerge = 0.156948;
    const double ref_final_rmerge = 0.152007;
    const double eps = 1.0e-5;

    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");
    ohkl::UnitCell* cell = experiment.getUnitCell("indexed");
    std::vector<ohkl::PeakCollection*> collections = {peaks};

    ohkl::MergedPeakCollection initial_merge(cell->spaceGroup(), collections, true, true);
    ohkl::RFactor initial_rfactor(true);
    initial_rfactor.calculate(&initial_merge);
    const double initial_rmerge = initial_rfactor.Rmerge();
    std::cout << "Initial Rmerge = " << initial_rfactor.Rmerge() << std::endl;
    CHECK_THAT(initial_rmerge, Catch::Matchers::WithinAbs(ref_initial_rmerge, eps));

    ohkl::Rescaler rescaler(peaks, cell->spaceGroup());
    auto* rescaler_parameters = rescaler.parameters();
    rescaler_parameters->sum_intensity = true;
    rescaler_parameters->friedel = true;

    std::optional<double> minf = rescaler.rescale();
    CHECK(minf);

    if (minf) {
        std::cout << rescaler.nIter() << " iterations" << std::endl;
        std::cout << "minf = " << minf.value() << std::endl;
        for (const auto& param : rescaler.scaleFactors())
            std::cout << param << " ";
    }
    std::cout << std::endl;

    ohkl::MergedPeakCollection final_merge(cell->spaceGroup(), collections, true, true);
    ohkl::RFactor final_rfactor(true);
    final_rfactor.calculate(&final_merge);
    const double final_rmerge = final_rfactor.Rmerge();
    std::cout << "Final Rmerge = " << final_rmerge << std::endl;
    CHECK_THAT(final_rmerge, Catch::Matchers::WithinAbs(ref_final_rmerge, eps));
}
