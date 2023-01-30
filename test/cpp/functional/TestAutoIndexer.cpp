//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestPeakFinder.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/DataSet.h"
#include "test/cpp/catch.hpp"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/loader/RawDataReader.h"
#include "core/convolve/AnnularConvolver.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakFilter.h"


#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

TEST_CASE("test/data/TestPeakFinder.cpp", "")
{
    const std::string filename = "CrChiA.ohkl";
    ohkl::Experiment experiment("test", "BioDiff2500");
    experiment.loadFromFile(filename);

    ohkl::UnitCell reference_cell;
    reference_cell.setParameters(
        57.96, 65.12, 86.52, 90.0 * ohkl::deg, 90.0 * ohkl::deg, 90.0 * ohkl::deg);
    reference_cell.setSpaceGroup(ohkl::SpaceGroup{"P 21 21 21"});

    auto data = experiment.getAllData()[0];
    auto* found_peaks = experiment.getPeakCollection("found");
    CHECK(found_peaks->numberOfPeaks() == 492);

    auto* indexer = experiment.autoIndexer();
    auto* params = indexer->parameters();
    params->maxdim = 100.0;
    params->nSolutions = 10;
    params->nVertices = 10000;
    params->subdiv = 30;
    params->indexingTolerance = 0.2;
    params->minUnitCellVolume = 10000;
    params->d_min = 1.5;
    params->d_max = 50;
    params->strength_min = 1.0;
    params->strength_max = 1.0e7;

    indexer->autoIndex(found_peaks);
    std::cout << indexer->solutionsToString() << std::endl;
    bool found_cell = false;
    for (const auto& [cell, quality] : indexer->solutions()) {
        if (!found_cell) {
            found_cell = reference_cell.isSimilar(cell.get(), 1.5, 0.1);
        }
    }
    CHECK(found_cell);

}
