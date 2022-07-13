//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/geometry/TestAABB.cpp
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

#include "base/geometry/AABB.h"

const double tolerance = 1e-5;

TEST_CASE("test/geometry/TestAABB.cpp", "")
{
    ohkl::AABB bb;
    bb.setLower(Eigen::Vector3d(0, 0, 0));
    bb.setUpper(Eigen::Vector3d(1, 2, 3));
    Eigen::Vector3d center, extends;
    center = bb.center();
    extends = bb.extents();

    CHECK(center[0] == Approx(0.5).epsilon(tolerance));
    CHECK(center[1] == Approx(1.0).epsilon(tolerance));
    CHECK(center[2] == Approx(1.5).epsilon(tolerance));

    CHECK(extends[0] == Approx(1).epsilon(tolerance));
    CHECK(extends[1] == Approx(2).epsilon(tolerance));
    CHECK(extends[2] == Approx(3).epsilon(tolerance));
    // Check that a given point is inside
    CHECK(bb.isInside(Eigen::Vector3d(center)));
    // Check that a given point is outside
    CHECK(!bb.isInside(Eigen::Vector3d(2, 3, 4)));
    // A second bounding box, just touching
    ohkl::AABB bb2(Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(2, 3, 4));
    CHECK(bb2.collide(bb));
    // Second bounding box overlaps .
    bb2.setLower(Eigen::Vector3d(0.5, 2, 3));
    bb2.setUpper(Eigen::Vector3d(2, 3, 4));
    CHECK(bb2.collide(bb));
    // No overlap
    bb2.setLower(Eigen::Vector3d(2, 3, 4));
    bb2.setUpper(Eigen::Vector3d(4, 5, 6));
    CHECK(!(bb2.collide(bb)));

    // Translate and scale operations
    bb.translate(Eigen::Vector3d(1, 2, 3));
    Eigen::Vector3d lower = bb.lower();
    Eigen::Vector3d upper = bb.upper();
    CHECK(lower[0] == Approx(1.0).epsilon(tolerance));
    CHECK(lower[1] == Approx(2.0).epsilon(tolerance));
    CHECK(lower[2] == Approx(3.0).epsilon(tolerance));
    CHECK(upper[0] == Approx(2.0).epsilon(tolerance));
    CHECK(upper[1] == Approx(4.0).epsilon(tolerance));
    CHECK(upper[2] == Approx(6.0).epsilon(tolerance));
}
