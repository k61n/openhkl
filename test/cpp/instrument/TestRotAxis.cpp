//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestRotAxis.cpp
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

#include "base/utils/Units.h"
#include "core/gonio/RotAxis.h"

const double tolerance = 1e-6;

TEST_CASE("test/instrument/TestRotAxis.cpp", "")
{
    // Empty Rotation axis initialize to (0,0,1)
    ohkl::RotAxis a("omega", Eigen::Vector3d(0, 0, 1));
    Eigen::Vector3d axis = a.axis();
    CHECK(axis[0] == 0);
    CHECK(axis[1] == 0);
    CHECK(axis[2] == 1);
    // CHECK(std::abs(a.getOffset(),tolerance);
    a.setRotationDirection(ohkl::RotAxis::Direction::CCW);
    // Check that a rotation CCW of 45.0 degrees brings the 1,0,0 vector into
    // 1/2.(sqrt(2),sqrt(2),0)
    Eigen::Vector3d transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * ohkl::deg);
    CHECK(transf[0] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(transf[1] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(std::abs(transf[2]) < tolerance);
    // Check same for CCW of 0,1,0
    transf = a.transform(Eigen::Vector3d(0, 1, 0), 45.0 * ohkl::deg);
    CHECK(transf[0] == Approx(-0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(transf[1] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(std::abs(transf[2]) < tolerance);

    // Switch rotation direction CW
    a.setRotationDirection(ohkl::RotAxis::Direction::CW);
    transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * ohkl::deg);
    CHECK(transf[0] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(transf[1] == Approx(-0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(std::abs(transf[2]) < tolerance);
    // Check same for CW of 0,1,0
    transf = a.transform(Eigen::Vector3d(0, 1, 0), 45.0 * ohkl::deg);
    CHECK(transf[0] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(transf[1] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(std::abs(transf[2]) < tolerance);

    // Switch rotation axis to y
    // a.setOffset(0.0);
    a.setAxis(Eigen::Vector3d(0, 1, 0));
    a.setRotationDirection(ohkl::RotAxis::Direction::CCW);
    transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * ohkl::deg);
    CHECK(transf[0] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(std::abs(transf[1]) < tolerance);
    CHECK(transf[2] == Approx(-0.5 * sqrt(2.0)).epsilon(tolerance));

    // Test with Homogeneous matrix
    transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * ohkl::deg);
    CHECK(transf[0] == Approx(0.5 * sqrt(2.0)).epsilon(tolerance));
    CHECK(std::abs(transf[1]) < tolerance);
    CHECK(transf[2] == Approx(-0.5 * sqrt(2.0)).epsilon(tolerance));
}
