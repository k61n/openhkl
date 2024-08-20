//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestIntegrationRegion.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/PixelSumIntegrator.h"
#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"

#include <iostream>

namespace ohkl {

TEST_CASE("test/integrate/TestIntegrationRegion.cpp", "")
{
    const double eps = 1.0e-5;

    const std::string filename = "CrChiA.ohkl";
    Experiment experiment("test", "BioDiff");
    experiment.loadFromFile(filename);

    sptrDataSet data = experiment.getData("testdata");
    PeakCollection* peaks = experiment.getPeakCollection("found");

    Peak3D* ref_peak = peaks->getPeakList()[382];

    std::cout << ref_peak->shape().center() << std::endl;

    IntegrationRegion region_fixed(ref_peak, 5.5, 1.3, 2.3, RegionType::FixedEllipsoid);

    for (std::size_t idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame = data->transformedFrame(idx);
        Eigen::MatrixXi mask;
        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));
        region_fixed.advanceFrame(current_frame, mask, idx);
    }

    const auto& events_fixed = region_fixed.peakData().events();
    const auto& counts_fixed = region_fixed.peakData().counts();
    std::size_t npixels_fixed = events_fixed.size();

    for (std::size_t idx = 0; idx < npixels_fixed; ++idx) {
        std::cout << idx << " " << events_fixed[idx].px << " " << events_fixed[idx].py <<  " "
                  << events_fixed[idx].frame << " " << counts_fixed[idx] << " " << std::endl;
    }
    std::cout << npixels_fixed << " pixels" << std::endl;

    CHECK(npixels_fixed == 319);

    CHECK(events_fixed[148].px == Approx(1447).epsilon(eps));
    CHECK(events_fixed[148].py == Approx(520).epsilon(eps));
    CHECK(events_fixed[148].frame == Approx(11).epsilon(eps));
    CHECK(counts_fixed[148] == Approx(1309).epsilon(eps));

    CHECK(events_fixed[216].px == Approx(1449).epsilon(eps));
    CHECK(events_fixed[216].py == Approx(523).epsilon(eps));
    CHECK(events_fixed[216].frame == Approx(12).epsilon(eps));
    CHECK(counts_fixed[216] == Approx(875).epsilon(eps));

    IntegrationRegion region_var(ref_peak, 3.0, 3.0, 6.0, RegionType::VariableEllipsoid);

    for (std::size_t idx = 0; idx < data->nFrames(); ++idx) {
        Eigen::MatrixXd current_frame = data->transformedFrame(idx);
        Eigen::MatrixXi mask;
        mask.resize(data->nRows(), data->nCols());
        mask.setConstant(int(IntegrationRegion::EventType::EXCLUDED));
        region_var.advanceFrame(current_frame, mask, idx);
    }

    const auto& events_var = region_var.peakData().events();
    const auto& counts_var = region_var.peakData().counts();
    std::size_t npixels_var = events_var.size();

    for (std::size_t idx = 0; idx < npixels_var; ++idx) {
        std::cout << idx << " " << events_var[idx].px << " " << events_var[idx].py <<  " "
                  << events_var[idx].frame << " " << counts_var[idx] << " " << std::endl;
    }
    std::cout << npixels_var << " pixels" << std::endl;

    CHECK(npixels_var == 242);

    CHECK(events_var[148].px == Approx(1446).epsilon(eps));
    CHECK(events_var[148].py == Approx(520).epsilon(eps));
    CHECK(events_var[148].frame == Approx(12).epsilon(eps));
    CHECK(counts_var[148] == Approx(637).epsilon(eps));

    CHECK(events_var[216].px == Approx(1451).epsilon(eps));
    CHECK(events_var[216].py == Approx(520).epsilon(eps));
    CHECK(events_var[216].frame == Approx(13).epsilon(eps));
    CHECK(counts_var[216] == Approx(696).epsilon(eps));
}
}
