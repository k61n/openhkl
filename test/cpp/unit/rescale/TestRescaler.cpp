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
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

#include "test/cpp/catch.hpp"

#include "core/rescale/Rescaler.h"

#include <iostream>


TEST_CASE("test/data/TestRescaler.cpp", "")
{
    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");
    ohkl::UnitCell* cell = experiment.getUnitCell("indexed");

    ohkl::Rescaler rescaler(peaks, cell->spaceGroup(), true, true);
    std::optional<double> minf = rescaler.rescale();
    CHECK(minf);

    if (minf) {
        std::cout << rescaler.nIter() << " iterations" << std::endl;
        std::cout << "minf = " << minf.value() << std::endl;
        for (const auto& param : rescaler.parameters())
            std::cout << param << " ";
    }
    std::cout << std::endl;

    CHECK(false);

}
