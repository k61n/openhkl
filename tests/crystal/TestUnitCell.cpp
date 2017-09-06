#define BOOST_TEST_MODULE "Test Unit Cell"
#define BOOST_TEST_DYN_LINK

#include <random>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/utils/Units.h>


const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Unit_Cell)
{
    double a=6.32;
    double b=7.22;
    double c=3.44;
    double alpha=90*nsx::deg;
    nsx::UnitCell cell(a,b,c,alpha,alpha,alpha);
    auto cc = cell.character();

    BOOST_CHECK_CLOSE(cc.a,a,tolerance);
    BOOST_CHECK_CLOSE(cc.b,b,tolerance);
    BOOST_CHECK_CLOSE(cc.c,c,tolerance);
    BOOST_CHECK_CLOSE(cc.alpha,alpha,tolerance);
    BOOST_CHECK_CLOSE(cc.beta,alpha,tolerance);
    BOOST_CHECK_CLOSE(cc.gamma,alpha,tolerance);

    BOOST_CHECK_CLOSE(cell.volume(),a*b*c,tolerance);

    const Eigen::Matrix3d& A=cell.basis();
    BOOST_CHECK_CLOSE(A(0,0),a,tolerance);
    BOOST_CHECK_SMALL(A(1,0),tolerance);
    BOOST_CHECK_SMALL(A(2,0),tolerance);
    BOOST_CHECK_SMALL(A(0,1),tolerance);
    BOOST_CHECK_CLOSE(A(1,1),b,tolerance);
    BOOST_CHECK_SMALL(A(2,1),tolerance);
    BOOST_CHECK_SMALL(A(0,2),tolerance);
    BOOST_CHECK_SMALL(A(1,2),tolerance);
    BOOST_CHECK_CLOSE(A(2,2),c,tolerance);

    const Eigen::Matrix3d& B=cell.reciprocalBasis();
    BOOST_CHECK_CLOSE(B(0,0),1/a,tolerance);
    BOOST_CHECK_SMALL(B(1,0),tolerance);
    BOOST_CHECK_SMALL(B(2,0),tolerance);
    BOOST_CHECK_SMALL(B(0,1),tolerance);
    BOOST_CHECK_CLOSE(B(1,1),1/b,tolerance);
    BOOST_CHECK_SMALL(B(2,1),tolerance);
    BOOST_CHECK_SMALL(B(0,2),tolerance);
    BOOST_CHECK_SMALL(B(1,2),tolerance);
    BOOST_CHECK_CLOSE(B(2,2),1/c,tolerance);


    const Eigen::Matrix3d& G=cell.metric();
    BOOST_CHECK_CLOSE(G(0,0),a*a,tolerance);
    BOOST_CHECK_SMALL(G(1,0),tolerance);
    BOOST_CHECK_SMALL(G(2,0),tolerance);
    BOOST_CHECK_SMALL(G(0,1),tolerance);
    BOOST_CHECK_CLOSE(G(1,1),b*b,tolerance);
    BOOST_CHECK_SMALL(G(2,1),tolerance);
    BOOST_CHECK_SMALL(G(0,2),tolerance);
    BOOST_CHECK_SMALL(G(1,2),tolerance);
    BOOST_CHECK_CLOSE(G(2,2),c*c,tolerance);

    cell.setLatticeCentring(nsx::LatticeCentring::I);
    cell.setBravaisType(nsx::BravaisType::Tetragonal);
    // Check angle calculations
    nsx::UnitCell cell4(10,10,10,90*nsx::deg,98*nsx::deg,90*nsx::deg);
    BOOST_CHECK_CLOSE(cell4.getAngle({1,0,0},{0,0,1}),82.0*nsx::deg,tolerance);

    // Check equivalence
    cell4.setSpaceGroup("P 4/m m m");

    BOOST_CHECK(cell4.isEquivalent(2,0,0,0,2,0));
    BOOST_CHECK(cell4.isEquivalent(2,3,2,3,2,-2));
    BOOST_CHECK(!cell4.isEquivalent(2,3,2,3,2,-3));


    // test covariance
    Eigen::Matrix3d AA;
    AA <<
    2.0, 1.0, 1.0,
    -0.5, 2.5,0.7,
    0.1, 0.2, 1.8;

    cell = nsx::UnitCell(AA);
    BOOST_CHECK_CLOSE(cell.volume(), std::fabs(AA.determinant()), 1e-10);

    Eigen::Matrix<double, 9, 9> cov;

    for (auto i = 0; i < 9; ++i) {
        for (auto j = 0; j < 9; ++j) {
            cov(i, j) = AA(i/3, i%3)*AA(j/3, j%3);
        }
    }

    cell.setCovariance(cov);

    for (auto i = 0; i < 3; ++i) {
        auto col = AA.col(i);
        auto row = AA.row(i);
        auto col_cov = cell.covariance(i);
        auto row_cov = cell.reciprocalCovariance(i);

        BOOST_CHECK_SMALL((col_cov-col*col.transpose()).norm(), 1e-10);
        BOOST_CHECK_SMALL((row_cov-row.transpose()*row).norm(), 1e-10);
    }

    cell.setCovariance(cov);
    auto rcov = cell.reciprocalCovariance();
    cell.setReciprocalCovariance(rcov);

    BOOST_CHECK_SMALL((cov-cell.covariance()).norm(), 1e-10);
    BOOST_CHECK_SMALL((rcov-cell.reciprocalCovariance()).norm(), 1e-10);

    cell.setReciprocalCovariance(cov);
    rcov = cell.covariance();
    cell.setCovariance(rcov);

    BOOST_CHECK_SMALL((rcov-cell.covariance()).norm(), 1e-10);
    BOOST_CHECK_SMALL((cov-cell.reciprocalCovariance()).norm(), 1e-10);

    // random number generator
    auto get_random = []()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::normal_distribution<> d(0,0.01);
        return d(gen);
    };

    // generate random unit cells, with some non-zero correlation in components
    // check that we correctly calculate the errors in the lattice character
    nsx::CellCharacter sigma_expected;

    cov *= 0.0;

    const int nmax = 1000;

    AA <<
    15.0, 1.0, 1.0,
    -0.5, 17.5,0.7,
    0.1, 0.2, 19.8;

    cell.setBasis(AA);
    auto A_cc = cell.character();
    
    for (auto n = 0; n < nmax; ++n) {
        Eigen::Matrix3d dA;

        for (auto j = 0; j < 3; ++j) {            
            for (auto k = 0; k < 3; ++k) {
                //dA(k, j) = (j-k)*d;
                dA(k, j) = get_random();
            }

            // introduce linear correlation by hand
            dA.col(2) += 0.3 * dA.col(0) + 0.2 * dA.col(1);
        }

        for (auto i = 0; i < 9; ++i) {
            for (auto j = 0; j < 9; ++j) {
                int a = i/3;
                int b = i%3;
                int c = j/3;
                int d = j%3;
                BOOST_CHECK_EQUAL(3*a+b, i);
                BOOST_CHECK_EQUAL(3*c+d, j);                
                cov(i, j) += dA(a,b)*dA(c,d);
                cov(j, i) = cov(i, j);
            }
        }

        auto AdA = A+dA;
        auto g = AdA.transpose()*AdA;
        nsx::CellCharacter dA_cc(g);

        sigma_expected.A += std::pow(A_cc.A-dA_cc.A, 2) / nmax;
        sigma_expected.B += std::pow(A_cc.B-dA_cc.B, 2) / nmax;
        sigma_expected.C += std::pow(A_cc.C-dA_cc.C, 2) / nmax;
        sigma_expected.D += std::pow(A_cc.D-dA_cc.D, 2) / nmax;
        sigma_expected.E += std::pow(A_cc.E-dA_cc.E, 2) / nmax;
        sigma_expected.F += std::pow(A_cc.F-dA_cc.F, 2) / nmax;

        sigma_expected.a += std::pow(A_cc.a-dA_cc.a, 2) / nmax;
        sigma_expected.b += std::pow(A_cc.b-dA_cc.b, 2) / nmax;
        sigma_expected.c += std::pow(A_cc.c-dA_cc.c, 2) / nmax;
        sigma_expected.alpha += std::pow(A_cc.alpha-dA_cc.alpha, 2) / nmax;
        sigma_expected.beta += std::pow(A_cc.beta-dA_cc.beta, 2) / nmax;
        sigma_expected.gamma += std::pow(A_cc.gamma-dA_cc.gamma, 2) / nmax;
    }

    cov /= nmax;

    cell.setBasis(AA);
    cell.setCovariance(cov);

    sigma_expected.A = std::sqrt(sigma_expected.A);
    sigma_expected.B = std::sqrt(sigma_expected.B);
    sigma_expected.C = std::sqrt(sigma_expected.C);
    sigma_expected.D = std::sqrt(sigma_expected.D);
    sigma_expected.E = std::sqrt(sigma_expected.E);
    sigma_expected.F = std::sqrt(sigma_expected.F);

    sigma_expected.a = std::sqrt(sigma_expected.a);
    sigma_expected.b = std::sqrt(sigma_expected.b);
    sigma_expected.c = std::sqrt(sigma_expected.c);
    sigma_expected.alpha = std::sqrt(sigma_expected.alpha);
    sigma_expected.beta = std::sqrt(sigma_expected.beta);
    sigma_expected.gamma = std::sqrt(sigma_expected.gamma);

    auto sigma = cell.characterSigmas();

    BOOST_CHECK_CLOSE(sigma.A, sigma_expected.A, 10.0);
    BOOST_CHECK_CLOSE(sigma.B, sigma_expected.B, 10.0);
    BOOST_CHECK_CLOSE(sigma.C, sigma_expected.C, 10.0);
    BOOST_CHECK_CLOSE(sigma.D, sigma_expected.D, 10.0);
    BOOST_CHECK_CLOSE(sigma.E, sigma_expected.E, 10.0);
    BOOST_CHECK_CLOSE(sigma.F, sigma_expected.F, 10.0);

    BOOST_CHECK_CLOSE(sigma.a, sigma_expected.a, 10.0);
    BOOST_CHECK_CLOSE(sigma.b, sigma_expected.b, 10.0);
    BOOST_CHECK_CLOSE(sigma.c, sigma_expected.c, 10.0);
    BOOST_CHECK_CLOSE(sigma.alpha, sigma_expected.alpha, 10.0);
    BOOST_CHECK_CLOSE(sigma.beta, sigma_expected.beta, 10.0);
    BOOST_CHECK_CLOSE(sigma.gamma, sigma_expected.gamma, 10.0);    

    // test niggli constraints
    cell.setParams(55.03, 58.60, 66.89, 1.569, 1.57, 1.571);
    int num = cell.reduce(false, 1e-2, 5e-3);
    BOOST_CHECK_EQUAL(num, 32);
    auto new_cell = cell.applyNiggliConstraints();
    auto ch = new_cell.character();

    BOOST_CHECK_CLOSE(ch.a, 55.03, 1e-3);
    BOOST_CHECK_CLOSE(ch.b, 58.60, 1e-3);
    BOOST_CHECK_CLOSE(ch.c, 66.89, 1e-3);

    BOOST_CHECK_CLOSE(ch.alpha, M_PI/2.0, 1e-6);
    BOOST_CHECK_CLOSE(ch.beta, M_PI/2.0, 1e-6);
    BOOST_CHECK_CLOSE(ch.gamma, M_PI/2.0, 1e-6);

    BOOST_CHECK_EQUAL(new_cell.equivalent(cell, 1e-3), true);
}
