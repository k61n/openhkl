#include <random>

#include <core/MillerIndex.h>
#include <core/Minimizer.h>
#include <core/NSXTest.h>
#include <core/SpaceGroup.h>
#include <core/UnitCell.h>
#include <core/Units.h>

const double tolerance = 1e-6;

NSX_INIT_TEST

int main()
{
    double a = 6.32;
    double b = 7.22;
    double c = 3.44;
    double alpha = 90 * nsx::deg;
    nsx::UnitCell cell(a, b, c, alpha, alpha, alpha);
    auto cc = cell.character();

    NSX_CHECK_CLOSE(cc.a, a, tolerance);
    NSX_CHECK_CLOSE(cc.b, b, tolerance);
    NSX_CHECK_CLOSE(cc.c, c, tolerance);
    NSX_CHECK_CLOSE(cc.alpha, alpha, tolerance);
    NSX_CHECK_CLOSE(cc.beta, alpha, tolerance);
    NSX_CHECK_CLOSE(cc.gamma, alpha, tolerance);

    NSX_CHECK_CLOSE(cell.volume(), a * b * c, tolerance);

    const Eigen::Matrix3d& A = cell.basis();
    NSX_CHECK_CLOSE(A(0, 0), a, tolerance);
    NSX_CHECK_SMALL(A(1, 0), tolerance);
    NSX_CHECK_SMALL(A(2, 0), tolerance);
    NSX_CHECK_SMALL(A(0, 1), tolerance);
    NSX_CHECK_CLOSE(A(1, 1), b, tolerance);
    NSX_CHECK_SMALL(A(2, 1), tolerance);
    NSX_CHECK_SMALL(A(0, 2), tolerance);
    NSX_CHECK_SMALL(A(1, 2), tolerance);
    NSX_CHECK_CLOSE(A(2, 2), c, tolerance);

    const Eigen::Matrix3d& B = cell.reciprocalBasis();
    NSX_CHECK_CLOSE(B(0, 0), 1 / a, tolerance);
    NSX_CHECK_SMALL(B(1, 0), tolerance);
    NSX_CHECK_SMALL(B(2, 0), tolerance);
    NSX_CHECK_SMALL(B(0, 1), tolerance);
    NSX_CHECK_CLOSE(B(1, 1), 1 / b, tolerance);
    NSX_CHECK_SMALL(B(2, 1), tolerance);
    NSX_CHECK_SMALL(B(0, 2), tolerance);
    NSX_CHECK_SMALL(B(1, 2), tolerance);
    NSX_CHECK_CLOSE(B(2, 2), 1 / c, tolerance);


    const Eigen::Matrix3d& G = cell.metric();
    NSX_CHECK_CLOSE(G(0, 0), a * a, tolerance);
    NSX_CHECK_SMALL(G(1, 0), tolerance);
    NSX_CHECK_SMALL(G(2, 0), tolerance);
    NSX_CHECK_SMALL(G(0, 1), tolerance);
    NSX_CHECK_CLOSE(G(1, 1), b * b, tolerance);
    NSX_CHECK_SMALL(G(2, 1), tolerance);
    NSX_CHECK_SMALL(G(0, 2), tolerance);
    NSX_CHECK_SMALL(G(1, 2), tolerance);
    NSX_CHECK_CLOSE(G(2, 2), c * c, tolerance);

    cell.setLatticeCentring(nsx::LatticeCentring::I);
    cell.setBravaisType(nsx::BravaisType::Tetragonal);
    // Check angle calculations
    nsx::UnitCell cell4(10, 10, 10, 90 * nsx::deg, 98 * nsx::deg, 90 * nsx::deg);
    NSX_CHECK_CLOSE(cell4.angle({1, 0, 0}, {0, 0, 1}), 82.0 * nsx::deg, tolerance);

    // Check equivalence
    cell4.setSpaceGroup(nsx::SpaceGroup("P 4/m m m"));

    auto space_group = cell4.spaceGroup();

    NSX_CHECK_ASSERT(
        space_group.isEquivalent(nsx::MillerIndex(2, 0, 0), nsx::MillerIndex(0, 2, 0)));
    NSX_CHECK_ASSERT(
        space_group.isEquivalent(nsx::MillerIndex(2, 3, 2), nsx::MillerIndex(3, 2, -2)));
    NSX_CHECK_ASSERT(
        !space_group.isEquivalent(nsx::MillerIndex(2, 3, 2), nsx::MillerIndex(3, 2, -3)));

    // test covariance
    Eigen::Matrix3d AA;
    AA << 2.0, 1.0, 1.0, -0.5, 2.5, 0.7, 0.1, 0.2, 1.8;

    cell = nsx::UnitCell(AA);
    NSX_CHECK_CLOSE(cell.volume(), std::fabs(AA.determinant()), 1e-10);

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
    nsx::UnitCellCharacter sigma_expected;
    const int nmax = 1000;

    AA << 15.0, 1.0, 1.0, -0.5, 17.5, 0.7, 0.1, 0.2, 19.8;

    cell.setBasis(AA);
    auto A_cc = cell.character();
    auto ch = cell.character();

    for (auto n = 0; n < nmax; ++n) {
        // random perturbation to character
        double d[6];

        for (auto i = 0; i < 6; ++i) {
            d[i] = get_random();
        }

        // update covariance matrix
        for (auto i = 0; i < 6; ++i) {
            for (auto j = 0; j < 6; ++j) {
                cov(i, j) += d[i] * d[j];
                cov(j, i) = cov(i, j);
            }
        }

        nsx::UnitCell new_cell = cell;
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
    NSX_CHECK_CLOSE(sigma.g00, sigma_expected.g00, 1e-3);
    NSX_CHECK_CLOSE(sigma.g01, sigma_expected.g01, 1e-3);
    NSX_CHECK_CLOSE(sigma.g02, sigma_expected.g02, 1e-3);
    NSX_CHECK_CLOSE(sigma.g11, sigma_expected.g11, 1e-3);
    NSX_CHECK_CLOSE(sigma.g12, sigma_expected.g12, 1e-3);
    NSX_CHECK_CLOSE(sigma.g22, sigma_expected.g22, 1e-3);

    // the sigmas of parameters a,b,c,alpha,beta,gamma are
    // computed by first order propagation of errors, so we do
    // not expect them to be so close to the true value
    NSX_CHECK_CLOSE(sigma.a, sigma_expected.a, 1e-1);
    NSX_CHECK_CLOSE(sigma.b, sigma_expected.b, 1e-1);
    NSX_CHECK_CLOSE(sigma.c, sigma_expected.c, 1e-1);
    NSX_CHECK_CLOSE(sigma.alpha, sigma_expected.alpha, 1.0);
    NSX_CHECK_CLOSE(sigma.beta, sigma_expected.beta, 1.0);
    NSX_CHECK_CLOSE(sigma.gamma, sigma_expected.gamma, 1.0);

    // test niggli constraints
    cell.setParameters(55.03, 58.60, 66.89, 1.569, 1.57, 1.571);
    int num = cell.reduce(false, 1e-2, 5e-3);
    NSX_CHECK_EQUAL(num, 32);
    auto new_cell = cell.applyNiggliConstraints();
    ch = new_cell.character();

    NSX_CHECK_CLOSE(ch.a, 55.03, 1e-3);
    NSX_CHECK_CLOSE(ch.b, 58.60, 1e-3);
    NSX_CHECK_CLOSE(ch.c, 66.89, 1e-3);

    NSX_CHECK_CLOSE(ch.alpha, M_PI / 2.0, 1e-6);
    NSX_CHECK_CLOSE(ch.beta, M_PI / 2.0, 1e-6);
    NSX_CHECK_CLOSE(ch.gamma, M_PI / 2.0, 1e-6);

    NSX_CHECK_EQUAL(new_cell.equivalent(cell, 1e-3), true);

    const double deg = M_PI / 180.0;
    nsx::NiggliCharacter nch;

    // from D19 reference data
    // this unit cell proved to be tricky

    // first check with Niggli only
    cell = nsx::UnitCell(5.557, 5.77, 16.138, 96.314 * deg, 90.0 * deg, 90.0 * deg);
    cell.reduce(true, 1e-2, 1e-3);
    nch = cell.niggliCharacter();
    NSX_CHECK_EQUAL(nch.number, 35);
    NSX_CHECK_NO_THROW(cell = cell.applyNiggliConstraints());

    // Niggli + Gruber
    cell = nsx::UnitCell(5.557, 5.77, 16.138, 96.314 * deg, 90.0 * deg, 90.0 * deg);
    cell.reduce(false, 1e-2, 1e-3);
    nch = cell.niggliCharacter();
    NSX_CHECK_EQUAL(nch.number, 35);
    NSX_CHECK_NO_THROW(cell = cell.applyNiggliConstraints());

    ch = cell.character();
    NSX_CHECK_CLOSE(ch.a, 5.76, 1.0);
    NSX_CHECK_CLOSE(ch.b, 5.55, 1.0);
    NSX_CHECK_CLOSE(ch.c, 16.12, 1.0);
    NSX_CHECK_CLOSE(ch.alpha, 90.0 * deg, 1.0);
    NSX_CHECK_CLOSE(ch.beta, 96.3 * deg, 1.0);
    NSX_CHECK_CLOSE(ch.gamma, 90.0 * deg, 1.0);

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

    nsx::FitParameters params;
    Eigen::Vector3d u;
    const Eigen::Matrix3d U = cell.niggliOrientation();
    Eigen::VectorXd x = cell.parameters();
    u.setZero();

    auto residual = [&](Eigen::VectorXd& f) -> int {
        nsx::UnitCell uc = cell.fromParameters(U, u, x);

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
    for (auto i = 0; i < 3; ++i) {
        params.addParameter(&u(i));
    }

    // try to fit the perturbed cell
    nsx::Minimizer min;
    min.initialize(params, 3 * q.size());
    min.set_f(residual);
    NSX_CHECK_EQUAL(min.fit(100), true);

    // new character of fitted cell
    ch = cell.fromParameters(U, u, x).character();

    NSX_CHECK_CLOSE(ch.a, 5.76, 1.0);
    NSX_CHECK_CLOSE(ch.b, 5.55, 1.0);
    NSX_CHECK_CLOSE(ch.c, 16.12, 1.0);
    NSX_CHECK_CLOSE(ch.alpha, 90.0 * deg, 1.0);
    NSX_CHECK_CLOSE(ch.beta, 96.3 * deg, 1.0);
    NSX_CHECK_CLOSE(ch.gamma, 90.0 * deg, 1.0);

    return 0;
}
