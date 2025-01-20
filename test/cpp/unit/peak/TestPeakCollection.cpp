//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestPeakCollection.cpp
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
#include "core/shape/PeakCollection.h"
#include "tables/crystal/MillerIndex.h"

#include <iostream>

TEST_CASE("test/integrate/TestPeakCollection.cpp", "")
{
    const double eps = 1.0e-3;

    const int ref_n_peaks = 6239;
    const int ref_valid_peaks = 5731;
    const int ref_invalid_peaks = 508;
    const double ref_sigma_d = 0.4150990052;
    const double ref_sigma_m = 0.2827556154;
    const ohkl::MillerIndex ref_index = {1, 14, -8};
    const ohkl::MillerIndex ref_symm = {-1, 14, -8};

    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    std::vector<ohkl::Peak3D*> peaks = experiment.getPeakCollection("predicted")->getPeakList();

    ohkl::PeakCollection collection("test", ohkl::PeakCollectionType::PREDICTED, data);
    collection.populate(peaks);
    CHECK(collection.numberOfPeaks() == ref_n_peaks);
    CHECK(collection.numberOfValid() == ref_valid_peaks);
    CHECK(collection.countEnabled() == ref_valid_peaks);
    CHECK(collection.numberOfInvalid() == ref_invalid_peaks);
    CHECK(collection.numberOfValid() + collection.numberOfInvalid() == ref_n_peaks);

    collection.computeSigmas();
    CHECK_THAT(collection.sigmaD(), Catch::Matchers::WithinAbs(ref_sigma_d, eps));
    CHECK_THAT(collection.sigmaM(), Catch::Matchers::WithinAbs(ref_sigma_m, eps));

    collection.getSymmetryRelated();

    ohkl::Peak3D* peak = collection.findPeakByIndex(ref_index);
    std::vector<ohkl::Peak3D*> symmetry_related = peak->symmetryRelated();
    CHECK(symmetry_related.at(0)->hkl() == ref_symm);
}
