//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/crystal/TestUnitCell.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <random>

#include "base/fit/Minimizer.h"
#include "base/utils/Units.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

const double tolerance = 1e-6;

TEST_CASE("test/crystal/TestUnitCell.cpp", "")
{
    double a = 6.32;
    double b = 7.22;
    double c = 3.44;
    double alpha = 90 * ohkl::deg;
    ohkl::UnitCell cell(a, b, c, alpha, alpha, alpha);
    auto cc = cell.character();

    CHECK(cc.a == Approx(a).epsilon(tolerance));
    CHECK(cc.b == Approx(b).epsilon(tolerance));
    CHECK(cc.c == Approx(c).epsilon(tolerance));
    CHECK(cc.alpha == Approx(alpha).epsilon(tolerance));
    CHECK(cc.beta == Approx(alpha).epsilon(tolerance));
    CHECK(cc.gamma == Approx(alpha).epsilon(tolerance));

    CHECK(cell.volume() == Approx(a * b * c).epsilon(tolerance));

    const Eigen::Matrix3d& A = cell.basis();
    CHECK(A(0, 0) == Approx(a).epsilon(tolerance));
    CHECK(std::abs(A(1, 0)) < tolerance);
    CHECK(std::abs(A(2, 0)) < tolerance);
    CHECK(std::abs(A(0, 1)) < tolerance);
    CHECK(A(1, 1) == Approx(b).epsilon(tolerance));
    CHECK(std::abs(A(2, 1)) < tolerance);
    CHECK(std::abs(A(0, 2)) < tolerance);
    CHECK(std::abs(A(1, 2)) < tolerance);
    CHECK(A(2, 2) == Approx(c).epsilon(tolerance));

    const Eigen::Matrix3d& B = cell.reciprocalBasis();
    CHECK(B(0, 0) == Approx(1 / a).epsilon(tolerance));
    CHECK(std::abs(B(1, 0)) < tolerance);
    CHECK(std::abs(B(2, 0)) < tolerance);
    CHECK(std::abs(B(0, 1)) < tolerance);
    CHECK(B(1, 1) == Approx(1 / b).epsilon(tolerance));
    CHECK(std::abs(B(2, 1)) < tolerance);
    CHECK(std::abs(B(0, 2)) < tolerance);
    CHECK(std::abs(B(1, 2)) < tolerance);
    CHECK(B(2, 2) == Approx(1 / c).epsilon(tolerance));

    const Eigen::Matrix3d& G = cell.metric();
    CHECK(G(0, 0) == Approx(a * a).epsilon(tolerance));
    CHECK(std::abs(G(1, 0)) < tolerance);
    CHECK(std::abs(G(2, 0)) < tolerance);
    CHECK(std::abs(G(0, 1)) < tolerance);
    CHECK(G(1, 1) == Approx(b * b).epsilon(tolerance));
    CHECK(std::abs(G(2, 1)) < tolerance);
    CHECK(std::abs(G(0, 2)) < tolerance);
    CHECK(std::abs(G(1, 2)) < tolerance);
    CHECK(G(2, 2) == Approx(c * c).epsilon(tolerance));

    cell.setLatticeCentring(ohkl::LatticeCentring::I);
    cell.setBravaisType(ohkl::BravaisType::Tetragonal);
    // Check angle calculations
    ohkl::UnitCell cell4(10, 10, 10, 90 * ohkl::deg, 98 * ohkl::deg, 90 * ohkl::deg);
    CHECK(cell4.angle({1, 0, 0}, {0, 0, 1}) == Approx(82.0 * ohkl::deg).epsilon(tolerance));

    // Check equivalence
    cell4.setSpaceGroup(ohkl::SpaceGroup("P 4/m m m"));

    auto space_group = cell4.spaceGroup();

    CHECK(space_group.isEquivalent(ohkl::MillerIndex(2, 0, 0), ohkl::MillerIndex(0, 2, 0)));
    CHECK(space_group.isEquivalent(ohkl::MillerIndex(2, 3, 2), ohkl::MillerIndex(3, 2, -2)));
    CHECK(!space_group.isEquivalent(ohkl::MillerIndex(2, 3, 2), ohkl::MillerIndex(3, 2, -3)));

    // test covariance
    Eigen::Matrix3d AA;
    AA << 2.0, 1.0, 1.0, -0.5, 2.5, 0.7, 0.1, 0.2, 1.8;

    cell = ohkl::UnitCell(AA);
    CHECK(cell.volume() == Approx(std::fabs(AA.determinant())).epsilon(1e-10));

    // computed covariance matrix
    Eigen::Matrix<double, 6, 6> cov;
    cov.setZero();

    // random number generator
    auto get_random = []() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::normal_distribution<> d(0, 0.01);
        return d(gen);
    };

    // generate random unit cells, with some non-zero correlation in components
    // check that we correctly calculate the errors in the lattice character
    ohkl::UnitCellCharacter sigma_expected;
    const int nmax = 1000;

    AA << 15.0, 1.0, 1.0, -0.5, 17.5, 0.7, 0.1, 0.2, 19.8;

    cell.setBasis(AA);
    auto A_cc = cell.character();
    auto ch = cell.character();

    for (auto n = 0; n < nmax; ++n) {
        // random perturbation to character
        double d[6];

        for (auto i = 0; i < 6; ++i)
            d[i] = get_random();

        // update covariance matrix
        for (auto i = 0; i < 6; ++i) {
            for (auto j = 0; j < 6; ++j) {
                cov(i, j) += d[i] * d[j];
                cov(j, i) = cov(i, j);
            }
        }

        ohkl::UnitCell new_cell = cell;
        new_cell.setMetric(
            ch.g00 + d[0], ch.g01 + d[5], ch.g02 + d[4], ch.g11 + d[1], ch.g12 + d[3],
            ch.g22 + d[2]);
        auto dA_cc = new_cell.character();

        sigma_expected.g00 += std::pow(A_cc.g00 - dA_cc.g00, 2) / nmax;
        sigma_expected.g01 += std::pow(A_cc.g01 - dA_cc.g01, 2) / nmax;
        sigma_expected.g02 += std::pow(A_cc.g02 - dA_cc.g02, 2) / nmax;
        sigma_expected.g11 += std::pow(A_cc.g11 - dA_cc.g11, 2) / nmax;
        sigma_expected.g12 += std::pow(A_cc.g12 - dA_cc.g12, 2) / nmax;
        sigma_expected.g22 += std::pow(A_cc.g22 - dA_cc.g22, 2) / nmax;

        sigma_expected.a += std::pow(A_cc.a - dA_cc.a, 2) / nmax;
        sigma_expected.b += std::pow(A_cc.b - dA_cc.b, 2) / nmax;
        sigma_expected.c += std::pow(A_cc.c - dA_cc.c, 2) / nmax;
        sigma_expected.alpha += std::pow(A_cc.alpha - dA_cc.alpha, 2) / nmax;
        sigma_expected.beta += std::pow(A_cc.beta - dA_cc.beta, 2) / nmax;
        sigma_expected.gamma += std::pow(A_cc.gamma - dA_cc.gamma, 2) / nmax;
    }

    cov /= nmax;

    cell.setBasis(AA);
    cell.setParameterCovariance(cov);

    sigma_expected.g00 = std::sqrt(sigma_expected.g00);
    sigma_expected.g01 = std::sqrt(sigma_expected.g01);
    sigma_expected.g02 = std::sqrt(sigma_expected.g02);
    sigma_expected.g11 = std::sqrt(sigma_expected.g11);
    sigma_expected.g12 = std::sqrt(sigma_expected.g12);
    sigma_expected.g22 = std::sqrt(sigma_expected.g22);

    sigma_expected.a = std::sqrt(sigma_expected.a);
    sigma_expected.b = std::sqrt(sigma_expected.b);
    sigma_expected.c = std::sqrt(sigma_expected.c);
    sigma_expected.alpha = std::sqrt(sigma_expected.alpha);
    sigma_expected.beta = std::sqrt(sigma_expected.beta);
    sigma_expected.gamma = std::sqrt(sigma_expected.gamma);

    auto sigma = cell.characterSigmas();

    // because of the way UnitCell computes sigmas,
    // for ABCDEF the sigma should be almost exact
    CHECK(sigma.g00 == Approx(sigma_expected.g00).epsilon(1e-3));
    CHECK(sigma.g01 == Approx(sigma_expected.g01).epsilon(1e-3));
    CHECK(sigma.g02 == Approx(sigma_expected.g02).epsilon(1e-3));
    CHECK(sigma.g11 == Approx(sigma_expected.g11).epsilon(1e-3));
    CHECK(sigma.g12 == Approx(sigma_expected.g12).epsilon(1e-3));
    CHECK(sigma.g22 == Approx(sigma_expected.g22).epsilon(1e-3));

    // the sigmas of parameters a,b,c,alpha,beta,gamma are
    // computed by first order propagation of errors, so we do
    // not expect them to be so close to the true value
    CHECK(sigma.a == Approx(sigma_expected.a).epsilon(1e-1));
    CHECK(sigma.b == Approx(sigma_expected.b).epsilon(1e-1));
    CHECK(sigma.c == Approx(sigma_expected.c).epsilon(1e-1));
    CHECK(sigma.alpha == Approx(sigma_expected.alpha).epsilon(1.0));
    CHECK(sigma.beta == Approx(sigma_expected.beta).epsilon(1.0));
    CHECK(sigma.gamma == Approx(sigma_expected.gamma).epsilon(1.0));

    // test niggli constraints
    cell.setParameters(55.03, 58.60, 66.89, 1.569, 1.57, 1.571);
    int num = cell.reduce(false, 1e-2, 5e-3);
    CHECK(num == 32);
    auto new_cell = cell.applyNiggliConstraints();
    ch = new_cell.character();

    CHECK(ch.a == Approx(55.03).epsilon(1e-3));
    CHECK(ch.b == Approx(58.60).epsilon(1e-3));
    CHECK(ch.c == Approx(66.89).epsilon(1e-3));

    CHECK(ch.alpha == Approx(M_PI / 2.0).epsilon(1e-6));
    CHECK(ch.beta == Approx(M_PI / 2.0).epsilon(1e-6));
    CHECK(ch.gamma == Approx(M_PI / 2.0).epsilon(1e-6));

    CHECK(new_cell.equivalent(cell, 1e-3) == true);

    const double deg = M_PI / 180.0;
    ohkl::NiggliCharacter nch;

    // from D19 reference data
    // this unit cell proved to be tricky

    // first check with Niggli only
    cell = ohkl::UnitCell(5.557, 5.77, 16.138, 96.314 * deg, 90.0 * deg, 90.0 * deg);
    cell.reduce(true, 1e-2, 1e-3);
    nch = cell.niggliCharacter();
    CHECK(nch.number == 35);
    CHECK_NOTHROW(cell = cell.applyNiggliConstraints());

    // Niggli + Gruber
    cell = ohkl::UnitCell(5.557, 5.77, 16.138, 96.314 * deg, 90.0 * deg, 90.0 * deg);
    cell.reduce(false, 1e-2, 1e-3);
    nch = cell.niggliCharacter();
    CHECK(nch.number == 35);
    CHECK_NOTHROW(cell = cell.applyNiggliConstraints());

    ch = cell.character();
    CHECK(ch.a == Approx(5.76).epsilon(1.0));
    CHECK(ch.b == Approx(5.55).epsilon(1.0));
    CHECK(ch.c == Approx(16.12).epsilon(1.0));
    CHECK(ch.alpha == Approx(90.0 * deg).epsilon(1.0));
    CHECK(ch.beta == Approx(96.3 * deg).epsilon(1.0));
    CHECK(ch.gamma == Approx(90.0 * deg).epsilon(1.0));

    // simulated data
    std::vector<Eigen::RowVector3d> q;
    std::vector<Eigen::RowVector3d> hkl;
    const int hkl_max = 10;

    for (int h = -hkl_max; h < hkl_max; ++h) {
        for (int k = -hkl_max; k < hkl_max; ++k) {
            for (int l = -hkl_max; l < hkl_max; ++l) {
                hkl.emplace_back(Eigen::RowVector3d(h, k, l));
                q.emplace_back(hkl.back() * cell.reciprocalBasis());
            }
        }
    }

    ohkl::FitParameters params;
    Eigen::Vector3d u;
    const Eigen::Matrix3d U = cell.niggliOrientation();
    Eigen::VectorXd x = cell.parameters();
    u.setZero();

    auto residual = [&](Eigen::VectorXd& f) -> int {
        ohkl::UnitCell uc = cell.fromParameters(U, u, x);

        for (size_t i = 0; i < q.size(); ++i) {
            auto dq = q[i] - hkl[i] * uc.reciprocalBasis();
            f(3 * i + 0) = dq(0);
            f(3 * i + 1) = dq(1);
            f(3 * i + 2) = dq(2);
        }
        return 0;
    };

    // perturb the cell slightly
    for (int i = 0; i < x.size(); ++i) {
        x(i) += x(i) * 0.1;
        params.addParameter(&x(i));
    }
    // add u to fit
    for (auto i = 0; i < 3; ++i)
        params.addParameter(&u(i));

    // try to fit the perturbed cell
    ohkl::Minimizer min;
    min.initialize(params, 3 * q.size());
    min.set_f(residual);
    CHECK(min.fit(100) == true);

    // new character of fitted cell
    ch = cell.fromParameters(U, u, x).character();

    CHECK(ch.a == Approx(5.76).epsilon(1.0));
    CHECK(ch.b == Approx(5.55).epsilon(1.0));
    CHECK(ch.c == Approx(16.12).epsilon(1.0));
    CHECK(ch.alpha == Approx(90.0 * deg).epsilon(1.0));
    CHECK(ch.beta == Approx(96.3 * deg).epsilon(1.0));
    CHECK(ch.gamma == Approx(90.0 * deg).epsilon(1.0));
}
