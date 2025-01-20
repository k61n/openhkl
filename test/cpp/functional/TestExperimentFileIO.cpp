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

#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/shape/PeakCollection.h"
#include "core/experiment/Experiment.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/Dense>

TEST_CASE("test/data/TestPeakFinder.cpp", "")
{
    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment expt_ref("Trypsin", "BioDiff");
    expt_ref.loadFromFile(filename);
    expt_ref.saveToFile("test.ohkl");

    auto* found_peaks_ref = expt_ref.getPeakCollection("found");
    auto* predicted_peaks_ref = expt_ref.getPeakCollection("predicted");
    auto* unit_cell_ref = expt_ref.getUnitCell("indexed");
    auto data_ref = expt_ref.getAllData()[0];

    ohkl::Experiment expt_test("test", "BioDiff");
    expt_test.loadFromFile("test.ohkl");
    auto* found_peaks_test = expt_test.getPeakCollection("found");
    auto* predicted_peaks_test = expt_test.getPeakCollection("predicted");
    auto* unit_cell_test = expt_test.getUnitCell("indexed");
    auto data_test = expt_test.getAllData()[0];

    CHECK(found_peaks_ref->numberOfPeaks() == found_peaks_test->numberOfPeaks());
    CHECK(predicted_peaks_ref->numberOfPeaks() == predicted_peaks_test->numberOfPeaks());
    CHECK(data_ref->nFrames() == data_test->nFrames());
    CHECK(unit_cell_ref->isSimilar(unit_cell_test, 0.01, 0.01) == true);

}
