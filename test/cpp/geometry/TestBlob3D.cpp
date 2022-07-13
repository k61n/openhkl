//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/geometry/TestBlob3D.cpp
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

#include "core/integration/Blob3D.h"

const double tolerance = 1e-5;

TEST_CASE("test/geometry/TestBlob3D.cpp", "")
{
    ohkl::Blob3D blob;
    // Create a set of points for a 3D Gaussian.
    double c_x = 12.0;
    double c_y = 15.0;
    double c_z = 22.0;
    double sx2 = 2.0, sy2 = 3.0, sz2 = 5.0;
    double prefactor = 1.0 / std::pow(2.0 * M_PI, 1.5) / sqrt(sx2 * sy2 * sz2);
    double tot = 0;
    for (int i = 0; i < 50; ++i) {
        for (int j = 0; j < 50; ++j) {
            for (int k = 0; k < 50; ++k) {
                double mass = prefactor
                    * exp(-0.5
                          * (std::pow(i - c_x, 2) / sx2 + std::pow(j - c_y, 2) / sy2
                             + std::pow(k - c_z, 2) / sz2));
                blob.addPoint(i, j, k, mass);
                tot += mass;
            }
        }
    }
    CHECK(tot == Approx(1.0).epsilon(tolerance));
    CHECK(tot == Approx(blob.getMass()).epsilon(tolerance));
    Eigen::Vector3d center, eigVal;
    Eigen::Matrix3d eigVec;
    double scale = 1.0;
    blob.toEllipsoid(scale, center, eigVal, eigVec);
    // Check that the center is OK
    CHECK(center(0) == Approx(c_x).epsilon(tolerance));
    CHECK(center(1) == Approx(c_y).epsilon(tolerance));
    CHECK(center(2) == Approx(c_z).epsilon(tolerance));
    // Check the semi_axes
    CHECK(eigVal(0) == Approx(sqrt(sx2)).epsilon(tolerance));
    CHECK(eigVal(1) == Approx(sqrt(sy2)).epsilon(tolerance));
    CHECK(eigVal(2) == Approx(sqrt(sz2)).epsilon(tolerance));

    // Check the eigenVectors
    CHECK(std::abs((eigVec.col(0))(0)) == Approx(1.0).epsilon(tolerance));
    CHECK(std::abs((eigVec.col(1))(1)) == Approx(1.0).epsilon(tolerance));
    CHECK(std::abs((eigVec.col(2))(2)) == Approx(1.0).epsilon(tolerance));
}
