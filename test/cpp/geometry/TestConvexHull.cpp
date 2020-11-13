//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/geometry/TestConvexHull.cpp
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

#include "base/geometry/ConvexHull.h"


TEST_CASE("test/geometry/TestConvexHull.cpp", "")
{
    const double tolerance = 1e-5;

    nsx::ConvexHull hull;
    hull.addVertex(Eigen::Vector3d(1, 1, 1));
    hull.addVertex(Eigen::Vector3d(-1, -1, -1));
    hull.addVertex(Eigen::Vector3d(-10, 0, 0));
    hull.addVertex(Eigen::Vector3d(10, 0, 0));
    hull.addVertex(Eigen::Vector3d(0, -10, 0));
    hull.addVertex(Eigen::Vector3d(0, 10, 0));
    hull.addVertex(Eigen::Vector3d(0, 0, -10));
    hull.addVertex(Eigen::Vector3d(0, 0, 10));
    hull.addVertex(Eigen::Vector3d(0, 0, 0));
    hull.addVertex(Eigen::Vector3d(0, 0, 5));
    hull.addVertex(Eigen::Vector3d(0, 5, 0));
    hull.addVertex(Eigen::Vector3d(5, 0, 0));

    bool ok = hull.updateHull(tolerance);

    CHECK(ok);

    CHECK(hull.vertices().size() == 6);
    CHECK(hull.faces().size() == 8);

    CHECK(hull.contains(Eigen::Vector3d(0, 0, 11)) == false);
    CHECK(hull.contains(Eigen::Vector3d(0, 10, 10)) == false);
    CHECK(hull.contains(Eigen::Vector3d(10, 10, 10)) == false);
    CHECK(hull.contains(Eigen::Vector3d(0, 0, 0)) == true);
    CHECK(hull.contains(Eigen::Vector3d(1, 1, 1)) == true);
}
