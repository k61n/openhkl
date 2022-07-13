//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/geometry/TestBrillouinZone.cpp
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

#include "tables/crystal/BrillouinZone.h"
#include "tables/crystal/UnitCell.h"

// note: validation data obtained from web, see e.g.
// http://lampx.tugraz.at/~hadley/ss1/bzones/
// https://en.wikipedia.org/wiki/Brillouin_zone

void validate_zone(const Eigen::Matrix3d& B, int nverts, int nfaces)
{
    ohkl::BrillouinZone zone(B, 1e-3);
    ohkl::ConvexHull hull = zone.convexHull();

    CHECK(hull.vertices().size() == nverts);
    CHECK(zone.vertices().size() == nverts);
    CHECK(2 * zone.normals().size() == nfaces);

    for (const auto& n : zone.normals()) {
        CHECK(zone.inside(0.5 * n));
        CHECK(zone.inside(-0.5 * n));

        CHECK(hull.contains(0.49 * n) == true);
        CHECK(hull.contains(0.51 * n) == false);

        CHECK(zone.inside(0.51 * n) == false);
        CHECK(zone.inside(-0.51 * n) == false);
    }

    for (const auto& v : zone.vertices()) {
        CHECK(zone.inside(v));
        CHECK(zone.inside(-v));

        CHECK(zone.inside(1.01 * v) == false);
        CHECK(zone.inside(-1.01 * v) == false);
    }

    // CHECK(hull.volume() == Approx(std::fabs(B.determinant())).epsilon(1e-8));
}

TEST_CASE("test/geometry/TestBrillouinZone.cpp", "")
{
    const double deg = M_PI / 180.0;
    ohkl::UnitCell uc;

    // todo: write this test!!
    Eigen::Matrix3d B;

    // primitive triclinic aP
    uc.setParameters(20, 25, 15, 95 * deg, 85 * deg, 93 * deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // face-centered cubic cF
    uc.setParameters(10, 10, 10, 30 * deg, 30 * deg, 30 * deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // primitive cubic cP
    uc.setParameters(10, 10, 10, 90 * deg, 90 * deg, 90 * deg);
    validate_zone(uc.reciprocalBasis(), 8, 6);

    // primitive monoclinic mP
    uc.setParameters(15, 20, 25, 78 * deg, 90 * deg, 90 * deg);
    validate_zone(uc.reciprocalBasis(), 12, 8);

    // face-centered monoclinic mC
    uc.setParameters(20, 20, 25, 78 * deg, 78 * deg, 85 * deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // primitive orthorhombic oP
    uc.setParameters(15, 20, 25, 90 * deg, 90 * deg, 90 * deg);
    validate_zone(uc.reciprocalBasis(), 8, 6);

    // face-centered orthorhombic oC

    // all faces centered orthorhombic oF

    // body-centered orthorhombic oI
    const double oI_beta = std::acos(0.5 * std::sqrt(15.0 / 20.0));
    uc.setParameters(15, 20, 20, 78 * deg, oI_beta, oI_beta);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // primitive tetragonal tP
    uc.setParameters(20, 25, 25, 90 * deg, 90 * deg, 90 * deg);
    validate_zone(uc.reciprocalBasis(), 8, 6);

    // body-centered tetragonal tI
    const double tI_alpha = std::acos(0.5 * 20.0 / 25.0);
    uc.setParameters(20, 20, 25, -tI_alpha, -tI_alpha, 90 * deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // hP
    // hR

    // cP
    // cF
    // cI
}
