//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/geometry/TestProfile3D.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <cmath>

#include <Eigen/Dense>

#include "base/geometry/AABB.h"
#include "core/shape/Profile3D.h"

TEST_CASE("test/geometry/TestProfile3D.cpp", "")
{
    ohkl::AABB bb;
    bb.setLower(Eigen::Vector3d(0, 0, 0));
    bb.setUpper(Eigen::Vector3d(1, 2, 3));

    ohkl::Profile3D profile(bb, 10, 10, 10);

    CHECK(profile.addValue({0., 0., 0.}, 1.0) == true);
    CHECK(profile.addValue({1., 2., 3.}, 1.0) == false);

    CHECK(profile.addValue({3., 3., 3.}, 1.0) == false);
    CHECK(profile.addValue({0.5, 1.0, 1.5}, 1.0) == true);

    CHECK(profile.addSubdividedValue({0.5, 1.0, 1.5}, 1.0, 5) == 125);
}
