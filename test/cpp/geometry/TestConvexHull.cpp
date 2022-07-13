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

#include "base/geometry/ConvexHull.h"
#include "test/cpp/catch.hpp"
#include <cmath>
#include <iostream>


TEST_CASE("test/geometry/TestConvexHull.cpp", "")
{
    const double tolerance = 1e-6;
    const double l = 12.3;

    ohkl::ConvexHull hull;
    hull.addVertex(Eigen::Vector3d(1, 1, 1));
    hull.addVertex(Eigen::Vector3d(-1, -1, -1));
    hull.addVertex(Eigen::Vector3d(-l, 0, 0));
    hull.addVertex(Eigen::Vector3d(l, 0, 0));
    hull.addVertex(Eigen::Vector3d(0, -l, 0));
    hull.addVertex(Eigen::Vector3d(0, l, 0));
    hull.addVertex(Eigen::Vector3d(0, 0, -l));
    hull.addVertex(Eigen::Vector3d(0, 0, l));
    hull.addVertex(Eigen::Vector3d(0, 0, 0));
    hull.addVertex(Eigen::Vector3d(0, 0, 5));
    hull.addVertex(Eigen::Vector3d(0, 5, 0));
    hull.addVertex(Eigen::Vector3d(5, 0, 0));

    bool ok = hull.updateHull(tolerance);
    CHECK(ok);

    // check octahedron with length l along axes
    CHECK(hull.vertices().size() == 6);
    CHECK(hull.faces().size() == 8);
    CHECK(hull.volume() == Approx(4. / 3. * l * l * l).epsilon(tolerance));

    CHECK(hull.contains(Eigen::Vector3d(-l - 1, 0, l + 1)) == false);
    CHECK(hull.contains(Eigen::Vector3d(0, 0, l + 1)) == false);
    CHECK(hull.contains(Eigen::Vector3d(0, l, l)) == false);
    CHECK(hull.contains(Eigen::Vector3d(l, l, l)) == false);
    CHECK(hull.contains(Eigen::Vector3d(0, 0, 0)) == true);
    CHECK(hull.contains(Eigen::Vector3d(1, 1, 1)) == true);
    CHECK(hull.contains(Eigen::Vector3d(-1, 1, -1)) == true);

    for (std::size_t faceidx = 0; faceidx < hull.faces().size(); ++faceidx) {
        const auto& face = hull.faces()[faceidx];
        const auto& norm = hull.normals()[faceidx];
        const double dist = hull.distances()[faceidx];

        std::cout << "face " << faceidx << " with " << face.size() << " vertices:" << std::endl;
        for (const auto& vert : face)
            std::cout << "\t" << vert.transpose() << std::endl;
        std::cout << "\tnormal: " << norm.transpose() << ", dist: " << dist << std::endl;
    }
    std::cout << std::endl;

    for (std::size_t vertidx = 0; vertidx < hull.vertices().size(); ++vertidx) {
        const auto& vert = hull.vertices()[vertidx];
        std::cout << "vertex " << vertidx << ": " << vert.transpose() << std::endl;
    }
}
