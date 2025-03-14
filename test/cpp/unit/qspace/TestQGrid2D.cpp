//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/unit/qspace/TestQGrid2D.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/qspace/QGrid2D.h"

#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"

#include <iostream>

TEST_CASE("test/qspace/TestQGrid2D.cpp", "")
{
    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);
    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::sptrUnitCell cell = experiment.getSptrUnitCell("indexed");

    ohkl::QGrid2D grid(data, cell);
    grid.initGrid(ohkl::Miller::l, 1.0, 0.001, 0.1);

    CHECK(false);
}
