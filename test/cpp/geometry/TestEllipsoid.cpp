//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/geometry/TestEllipsoid.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/geometry/AABB.h"
#include "base/geometry/Ellipsoid.h"

#include <Eigen/Dense>
#include <iostream>

const double eps = 1e-8;

TEST_CASE("test/geometry/TestEllipsoid.cpp", "")
{
    Eigen::Vector3d center(10, 10, 10);
    Eigen::Vector3d semi_axes(3, 3, 4);
    Eigen::Matrix3d eigV;
    eigV << 1, 0, 0, 0, 1, 0, 0, 0, 1;
    ohkl::Ellipsoid e(center, semi_axes, eigV);
    ohkl::Ellipsoid f(center, semi_axes, eigV);

    auto p = e.aabb().center();

    CHECK(p(0) == Approx(center(0)).epsilon(eps));
    CHECK(p(1) == Approx(center(1)).epsilon(eps));
    CHECK(p(2) == Approx(center(2)).epsilon(eps));

    auto lower = e.aabb().lower();

    CHECK(lower(0) == Approx(-3 + center(0)).epsilon(eps));
    CHECK(lower(1) == Approx(-3 + center(1)).epsilon(eps));
    CHECK(lower(2) == Approx(-4 + center(2)).epsilon(eps));

    auto upper = e.aabb().upper();

    CHECK(upper(0) == Approx(3 + center(0)).epsilon(eps));
    CHECK(upper(1) == Approx(3 + center(1)).epsilon(eps));
    CHECK(upper(2) == Approx(4 + center(2)).epsilon(eps));

    auto extents = e.aabb().extents();

    CHECK(extents(0) == Approx(6).epsilon(eps));
    CHECK(extents(1) == Approx(6).epsilon(eps));
    CHECK(extents(2) == Approx(8).epsilon(eps));

    CHECK(e.volume() == Approx(3 * 3 * 4 * 4 * M_PI / 3.0).epsilon(eps));

    Eigen::Vector3d shift(1, -1, 2);

    e.translate(shift);

    p = e.aabb().center();

    CHECK(p(0) == Approx(center(0) + shift(0)).epsilon(eps));
    CHECK(p(1) == Approx(center(1) + shift(1)).epsilon(eps));
    CHECK(p(2) == Approx(center(2) + shift(2)).epsilon(eps));

    lower = e.aabb().lower();

    CHECK(lower(0) == Approx(-3 + center(0) + shift(0)).epsilon(eps));
    CHECK(lower(1) == Approx(-3 + center(1) + shift(1)).epsilon(eps));
    CHECK(lower(2) == Approx(-4 + center(2) + shift(2)).epsilon(eps));

    upper = e.aabb().upper();

    CHECK(upper(0) == Approx(3 + center(0) + shift(0)).epsilon(eps));
    CHECK(upper(1) == Approx(3 + center(1) + shift(1)).epsilon(eps));
    CHECK(upper(2) == Approx(4 + center(2) + shift(2)).epsilon(eps));

    extents = e.aabb().extents();

    CHECK(extents(0) == Approx(6).epsilon(eps));
    CHECK(extents(1) == Approx(6).epsilon(eps));
    CHECK(extents(2) == Approx(8).epsilon(eps));

    CHECK(e.volume() == Approx(3 * 3 * 4 * M_PI * 4 / 3.0).epsilon(eps));

    const int N = 8;
    const double dx = 4.5 / N;

    int count = 0;

    for (auto i = -N; i < N; ++i) {
        for (auto j = -N; j < N; ++j) {
            for (auto k = -N; k < N; ++k) {
                Eigen::Vector3d u(center(0) + i * dx, center(1) + j * dx, center(2) + k * dx);

                // TODO restore test CHECK(f.isInside(u), (u - center).transpose() * f.metric() * (u
                // - center) < 1.0);

                if (f.isInside(u)) {
                    ++count;
                }
            }
        }
    }

    CHECK(count * dx * dx * dx == Approx(f.volume()).epsilon(0.1));

    e = f;
    shift << 6.1, 0, 0;
    f.translate(shift);
    CHECK(!e.collide(f));
    CHECK(!f.collide(e));
    f.translate(-shift / 2);
    CHECK(e.collide(f));
    CHECK(f.collide(e));
    f.translate(-shift / 2);

    auto fCenter = f.aabb().center();

    CHECK(fCenter[0] == Approx(center(0)).epsilon(eps));
    CHECK(fCenter[1] == Approx(center(1)).epsilon(eps));
    CHECK(fCenter[2] == Approx(center(2)).epsilon(eps));

    e.translate(-shift);

    // AABB center at -18,-18,-18
    ohkl::AABB aabb(Eigen::Vector3d(-20, -20, -20), Eigen::Vector3d(-16, -16, -16));
    CHECK(!f.collide(aabb));

    // AABB center at -16,-16,-16
    aabb.translate(Eigen::Vector3d(2, 2, 2));
    CHECK(!f.collide(aabb));

    // AABB center at 0,0,0
    aabb.translate(Eigen::Vector3d(16, 16, 16));
    CHECK(!f.collide(aabb));

    // AABB center at 10,10,10
    aabb.translate(Eigen::Vector3d(10, 10, 10));
    CHECK(f.collide(aabb));

    // AABB center at 12,10,10
    aabb.translate(Eigen::Vector3d(2, 0, 0));
    CHECK(f.collide(aabb));

    // AABB center at 14,10,10
    aabb.translate(Eigen::Vector3d(2, 0, 0));
    CHECK(f.collide(aabb));

    // AABB center at 16,10,10
    aabb.translate(Eigen::Vector3d(2, 0, 0));
    CHECK(!f.collide(aabb));

    // test inverse homogeneous matrix
    Eigen::Vector3d c(15.4, 12.2, 7.5);
    Eigen::Matrix3d m;

    m << 16.0, 4.0, -1.0, 4.0, 20.0, -3.0, -1.0, -3.0, 10.0;

    ohkl::Ellipsoid g(c, m);

    Eigen::Matrix4d Q = g.homogeneousMatrix();
    Eigen::Matrix4d QI = g.homogeneousMatrixInverse();
    Eigen::Matrix4d I = Eigen::Matrix4d::Identity();

    Eigen::Matrix4d R = Q.inverse();

    const double eps = 1e-10;

    CHECK(std::abs((Q * R - I).norm()) < eps);
    CHECK(std::abs((R * Q - I).norm()) < eps);
    CHECK(std::abs((Q * QI - I).norm()) < eps);
    CHECK(std::abs((QI * Q - I).norm()) < eps);
    CHECK(std::abs((QI - R).norm()) < eps);

    std::cout << "--------------" << std::endl;
    std::cout << Q.inverse() << std::endl;
    std::cout << "--------------" << std::endl;
    std::cout << QI << std::endl;

    ohkl::AABB box(Eigen::Vector3d(0, 0, 0), Eigen::Vector3d(1, 1, 1));

    const double t = 1.0 / std::sqrt(2);
    Eigen::Matrix3d U;

    U << -t, t, 0, t, t, 0, 0, 0, 1;

    ohkl::Ellipsoid el(Eigen::Vector3d(3, -3, 0), Eigen::Vector3d(1.0, 20.0, 100.0), U);

    // check collision with line segments
    CHECK(el.collideSegment(Eigen::Vector3d(3, -3, 0), Eigen::Vector3d(3, -100, 0)));
    CHECK(el.collideSegment(Eigen::Vector3d(5, -5, 0), Eigen::Vector3d(1, -1, 0)));

    // this test fails with the old incorrect collision
    CHECK(el.aabb().collide(box) == true);
    CHECK(box.collide(el) == false);
    CHECK(el.collide(box) == false);
}
