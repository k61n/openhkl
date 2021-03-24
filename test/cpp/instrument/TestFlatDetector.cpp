//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/instrument/TestFlatDetector.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <cmath>

#include "base/geometry/DirectVector.h"
#include "base/utils/Units.h"
#include "core/detector/FlatDetector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/InstrumentState.h"

const double tolerance = 1e-3;

TEST_CASE("test/instrument/TestFlatDetector.cpp", "")
{
    nsx::FlatDetector d("D10-detector");
    d.setDistance(380 * nsx::mm);
    d.setWidth(80 * nsx::mm);
    d.setHeight(80 * nsx::mm);
    d.setNCols(32);
    d.setNRows(32);

    // This should be the center of the detector at rest at (0,0.764,0)
    const nsx::DirectVector pixel_position = d.pixelPosition(15.5, 15.5);

    const Eigen::Vector3d& center = pixel_position.vector();
    CHECK(std::abs(center[0]) < tolerance);
    CHECK(center[1] == Approx(0.380).epsilon(tolerance));
    CHECK(std::abs(center[2]) < tolerance);

    // Create a fake instrument state
    const nsx::InstrumentState state1(nullptr);

    CHECK(std::abs(state1.gamma(pixel_position)) < tolerance);
    CHECK(std::abs(state1.nu(pixel_position)) < tolerance);
    CHECK(std::abs(state1.twoTheta(pixel_position)) < tolerance);

    // Rotate the detector by 90 deg clockwise
    nsx::InstrumentState state2(nullptr);

    state2.detectorOrientation << 0, 1, 0, -1, 0, 0, 0, 0, 1;

    CHECK(state2.gamma(pixel_position) == Approx(90 * nsx::deg).epsilon(tolerance));
    CHECK(std::abs(state2.nu(pixel_position)) < tolerance);
    CHECK(state2.twoTheta(pixel_position) == Approx(90.0 * nsx::deg).epsilon(tolerance));

    const nsx::ReciprocalVector kf = state2.kfLab(pixel_position);

    // Scattering in the center of the detector with wavelength 2.0
    // TODO restore CHECK(kf[0] == Approx(1).epsilon(0, tolerance));
    CHECK(std::abs(kf[1]) < tolerance);
    CHECK(std::abs(kf[2]) < tolerance);

    const nsx::ReciprocalVector q = state2.sampleQ(pixel_position);

    // Should be 45 deg in the x,-y plane
    CHECK(q[0] == Approx(1.0).epsilon(tolerance));
    CHECK(q[1] == Approx(-1.0).epsilon(tolerance));
    CHECK(std::abs(q[2]) < tolerance);

    for (int i = d.minRow() + 1; i < d.maxRow() - 1; i++) {
        for (int j = d.minCol() + 1; j < d.maxCol() - 1; j++) {
            CHECK(d.hasPixel(j, i));

            auto position = d.pixelPosition(j, i);

            Eigen::Vector3d from(-1, -1, -1);
            from *= nsx::cm;
            const Eigen::Vector3d kf = position.vector() - from;

            const nsx::DetectorEvent event = d.constructEvent(
                nsx::DirectVector(from), nsx::ReciprocalVector(kf.transpose()), 0.);

            // detector has event
            CHECK(event._tof > 0.0);
            // time of flight is correct
            CHECK(event._tof == Approx(1.0).epsilon(1e-5));
            // correct x coord
            CHECK(event._px == Approx(j).epsilon(1e-5));
            // correct x coord
            CHECK(event._py == Approx(i).epsilon(1e-5));
        }
    }
}
