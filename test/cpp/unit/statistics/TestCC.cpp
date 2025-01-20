//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestCC.cpp
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
#include "tables/crystal/UnitCell.h"
#include "core/statistics/CC.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "tables/crystal/MillerIndex.h"

#include <iostream>

TEST_CASE("test/integrate/TestCC.cpp", "")
{
    const double eps = 1.0e-2;

    const double ref_cchalf = 0.9595113149;
    const double ref_ccstar = 0.9896147512;

    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");
    ohkl::UnitCell* cell = experiment.getUnitCell("indexed");

    std::vector<ohkl::PeakCollection*> peak_collections = {peaks};
    ohkl::MergedPeakCollection merged_peaks(cell->spaceGroup(), peak_collections, true, true);

    ohkl::CC cc(true);
    cc.calculate(&merged_peaks);


    CHECK_THAT(cc.CChalf(), Catch::Matchers::WithinAbs(ref_cchalf, eps));
    CHECK_THAT(cc.CCstar(), Catch::Matchers::WithinAbs(ref_ccstar, eps));
}
