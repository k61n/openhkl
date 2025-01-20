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

    const int peak_index = 1842;
    const int event1_index = 72;
    const int event2_index = 131;

    const int ref_npixels_fixed = 270;
    const double ref_fixed_event1_px = 1476;
    const double ref_fixed_event1_py = 525;
    const double ref_fixed_event1_frame = 3;
    const double ref_fixed_event1_counts = 696;
    const double ref_fixed_event2_px = 1478;
    const double ref_fixed_event2_py = 522;
    const double ref_fixed_event2_frame = 4;
    const double ref_fixed_event2_counts = 2910;

    const int ref_npixels_variable = 223;
    const double ref_variable_event1_px = 1474;
    const double ref_variable_event1_py = 523;
    const double ref_variable_event1_frame = 4;
    const double ref_variable_event1_counts = 755;
    const double ref_variable_event2_px = 1480;
    const double ref_variable_event2_py = 522;
    const double ref_variable_event2_frame = 4;
    const double ref_variable_event2_counts = 1382;


    const std::string filename = "Trypsin-small.ohkl";
    Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);

    sptrDataSet data = experiment.getData("Scan I");
    PeakCollection* peaks = experiment.getPeakCollection("predicted");

    Peak3D* ref_peak = peaks->getPeakList()[peak_index];

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

    CHECK(npixels_fixed == ref_npixels_fixed);

    CHECK(events_fixed[event1_index].px == Approx(ref_fixed_event1_px).epsilon(eps));
    CHECK(events_fixed[event1_index].py == Approx(ref_fixed_event1_py).epsilon(eps));
    CHECK(events_fixed[event1_index].frame == Approx(ref_fixed_event1_frame).epsilon(eps));
    CHECK(counts_fixed[event1_index] == Approx(ref_fixed_event1_counts).epsilon(eps));

    CHECK(events_fixed[event2_index].px == Approx(ref_fixed_event2_px).epsilon(eps));
    CHECK(events_fixed[event2_index].py == Approx(ref_fixed_event2_py).epsilon(eps));
    CHECK(events_fixed[event2_index].frame == Approx(ref_fixed_event2_frame).epsilon(eps));
    CHECK(counts_fixed[event2_index] == Approx(ref_fixed_event2_counts).epsilon(eps));

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

    CHECK(npixels_var == ref_npixels_variable);

    CHECK(events_var[event1_index].px == Approx(ref_variable_event1_px).epsilon(eps));
    CHECK(events_var[event1_index].py == Approx(ref_variable_event1_py).epsilon(eps));
    CHECK(events_var[event1_index].frame == Approx(ref_variable_event1_frame).epsilon(eps));
    CHECK(counts_var[event1_index] == Approx(ref_variable_event1_counts).epsilon(eps));

    CHECK(events_var[event2_index].px == Approx(ref_variable_event2_px).epsilon(eps));
    CHECK(events_var[event2_index].py == Approx(ref_variable_event2_py).epsilon(eps));
    CHECK(events_var[event2_index].frame == Approx(ref_variable_event2_frame).epsilon(eps));
    CHECK(counts_var[event2_index] == Approx(ref_variable_event2_counts).epsilon(eps));
}
}
