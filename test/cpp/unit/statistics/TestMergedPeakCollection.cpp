//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestMergedPeakCollection.cpp
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
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include <iostream>

TEST_CASE("test/integrate/TestMergedPeakCollection.cpp", "")
{
    const double eps = 1.0e-3;
    const int eps_peaks = 10;

    const int ref_npeaks = 5731;
    const int ref_merged = 4880;
    const int ref_max_peaks = 35149;
    const double ref_redundancy = 1.1743852459;

    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");
    ohkl::UnitCell* cell = experiment.getUnitCell("indexed");

    std::vector<ohkl::PeakCollection*> peak_collections = {peaks};
    ohkl::MergedPeakCollection merged_peaks(cell->spaceGroup(), peak_collections, true, true);

    merged_peaks.setDRange(1.5, 50.0);


    CHECK(merged_peaks.totalSize() >= ref_npeaks - eps_peaks);
    CHECK(merged_peaks.totalSize() <= ref_npeaks + eps_peaks);
    CHECK(merged_peaks.nUnique() >= ref_merged - eps_peaks);
    CHECK(merged_peaks.nUnique() <= ref_merged + eps_peaks);
    CHECK(merged_peaks.maxPeaks() == ref_max_peaks);
    CHECK_THAT(merged_peaks.redundancy(), Catch::Matchers::WithinAbs(ref_redundancy, eps));
}
