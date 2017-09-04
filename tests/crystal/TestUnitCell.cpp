#define BOOST_TEST_MODULE "Test Unit Cell"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <cmath>

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
    nsx::CellCharacter ch = cell.character();

    BOOST_CHECK_CLOSE(ch.a,a,tolerance);
    BOOST_CHECK_CLOSE(ch.b,b,tolerance);
    BOOST_CHECK_CLOSE(ch.c,c,tolerance);
    BOOST_CHECK_CLOSE(ch.alpha,alpha,tolerance);
    BOOST_CHECK_CLOSE(ch.beta,alpha,tolerance);
    BOOST_CHECK_CLOSE(ch.gamma,alpha,tolerance);

    BOOST_CHECK_CLOSE(cell.volume(),a*b*c,tolerance);

    const Eigen::Matrix3d& A = cell.basis();
    BOOST_CHECK_CLOSE(A(0,0),a,tolerance);
    BOOST_CHECK_SMALL(A(1,0),tolerance);
    BOOST_CHECK_SMALL(A(2,0),tolerance);
    BOOST_CHECK_SMALL(A(0,1),tolerance);
    BOOST_CHECK_CLOSE(A(1,1),b,tolerance);
    BOOST_CHECK_SMALL(A(2,1),tolerance);
    BOOST_CHECK_SMALL(A(0,2),tolerance);
    BOOST_CHECK_SMALL(A(1,2),tolerance);
    BOOST_CHECK_CLOSE(A(2,2),c,tolerance);

    const Eigen::Matrix3d& B = cell.reciprocalBasis();
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
    auto q1 = cell4.fromIndex({1, 0, 0});
    auto q2 = cell4.fromIndex({0, 0, 1});
    BOOST_CHECK_CLOSE(std::acos(q1.dot(q2) / q1.norm() / q2.norm()),82.0*nsx::deg,tolerance);

    // Check equivalence
    cell4.setSpaceGroup("P 4/m m m");

    BOOST_CHECK(cell4.isEquivalent(2,0,0,0,2,0));
    BOOST_CHECK(cell4.isEquivalent(2,3,2,3,2,-2));
    BOOST_CHECK(!cell4.isEquivalent(2,3,2,3,2,-3));

}
