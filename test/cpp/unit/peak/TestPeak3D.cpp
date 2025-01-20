//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestPeak3D.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/geometry/ReciprocalVector.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "tables/crystal/MillerIndex.h"

#include <iostream>

TEST_CASE("test/integrate/TestPeak3D.cpp", "")
{
    const double eps = 1.0e-5;

    const ohkl::MillerIndex ref_index = {1, 14, -8};
    const ohkl::MillerIndex ref_symm = {-1, 14, -8};
    const int ref_multplicity = 2;
    const ohkl::ReciprocalVector ref_q(-0.2402793776, -0.1115494776, 0.0387136038);
    const double ref_d = 3.7351881942;
    const double ref_intensity = 4103.0451326598;
    const double ref_sigma = 12.755043599;
    const double ref_corrected_intensity = 145220.3562170387;
    const double ref_corrected_sigma = 451.4432366987;

    const std::string filename = "Trypsin-small.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("predicted");
    peaks->getSymmetryRelated();

    ohkl::Peak3D* peak = peaks->findPeakByIndex(ref_index);
    std::vector<ohkl::Peak3D*> symmetry_related = peak->symmetryRelated();
    CHECK(symmetry_related.at(0)->hkl() == ref_symm);
    CHECK(peak->multiplicity() == ref_multplicity);

    CHECK_THAT(peak->q()[0], Catch::Matchers::WithinAbs(ref_q[0], eps));
    CHECK_THAT(peak->q()[1], Catch::Matchers::WithinAbs(ref_q[1], eps));
    CHECK_THAT(peak->q()[2], Catch::Matchers::WithinAbs(ref_q[2], eps));
    CHECK_THAT(peak->d(), Catch::Matchers::WithinAbs(ref_d, eps));
    CHECK_THAT(peak->sumIntensity().value(), Catch::Matchers::WithinAbs(ref_intensity, eps));
    CHECK_THAT(peak->sumIntensity().sigma(), Catch::Matchers::WithinAbs(ref_sigma, eps));
    CHECK_THAT(
        peak->correctedSumIntensity().value(),
        Catch::Matchers::WithinAbs(ref_corrected_intensity, eps));
    CHECK_THAT(
        peak->correctedSumIntensity().sigma(),
        Catch::Matchers::WithinAbs(ref_corrected_sigma, eps));
}
