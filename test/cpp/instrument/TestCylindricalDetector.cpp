//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/instrument/TestCylindricalDetector.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <cmath>

#include <Eigen/Dense>

#include "base/utils/Units.h"
#include "core/detector/CylindricalDetector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/InstrumentState.h"

const double tolerance = 1e-3;

TEST_CASE("test/instrument/TestCylindricalDetector.cpp", "")
{
    nsx::CylindricalDetector d("D19-detector");
    d.setDistance(764 * nsx::mm);
    d.setAngularWidth(120.0 * nsx::deg);
    d.setHeight(40.0 * nsx::cm);
    d.setNCols(640);
    d.setNRows(256);

    const nsx::DirectVector pixel_position = d.pixelPosition(319.5, 127.5);

    // This should be the center of the detector at rest at (0,0.764,0)
    const Eigen::Vector3d& center = pixel_position.vector();
    CHECK(std::abs(center[0]) < tolerance);
    CHECK(center[1] == Approx(0.764).epsilon(tolerance));
    CHECK(std::abs(center[2]) < tolerance);

    // Create a fake instrument state
    const nsx::InstrumentState state1(nullptr);

    // Should be center of the detector so gamma,nu=0 at rest
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

    CHECK(kf[0] == Approx(1).epsilon(tolerance)); // may be converted incorrectly
    CHECK(std::abs(kf[1]) < tolerance);
    CHECK(std::abs(kf[2]) < tolerance);

    const nsx::ReciprocalVector q = state2.sampleQ(pixel_position);

    CHECK(q[0] == Approx(1.0).epsilon(tolerance));
    CHECK(q[1] == Approx(-1.0).epsilon(tolerance));
    CHECK(std::abs(q[2]) < tolerance);

    for (int i = d.minRow() + 3; i < d.maxRow() - 3; i += 2) {
        for (int j = d.minCol() + 3; j < d.maxCol() - 3; j += 2) {
            CHECK(d.hasPixel(j, i));

            auto position = d.pixelPosition(j, i);

            Eigen::Vector3d from(-1, -1, -1);
            from *= nsx::cm;
            const Eigen::Vector3d kf = position.vector() - from;

            const nsx::DetectorEvent event = d.constructEvent(
                nsx::DirectVector(from), nsx::ReciprocalVector(kf.transpose()), 0.);

            // detector has event
            CHECK(event.tof > 0.0);
            // time of flight is correct
            CHECK(event.tof == Approx(1.0).epsilon(1e-5));
            // correct x coord
            CHECK(event.px == Approx(j).epsilon(1e-5));
            // correct x coord
            CHECK(event.py == Approx(i).epsilon(1e-5));
        }
    }
}
