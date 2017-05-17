#define BOOST_TEST_MODULE "Test Unit Cell"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/utils/Units.h>

using namespace nsx;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Unit_Cell)
{
    double a=6.32;
    double b=7.22;
    double c=3.44;
    double alpha=90*deg;
    UnitCell cell(a,b,c,alpha,alpha,alpha);

    BOOST_CHECK_CLOSE(cell.getA(),a,tolerance);
    BOOST_CHECK_CLOSE(cell.getB(),b,tolerance);
    BOOST_CHECK_CLOSE(cell.getC(),c,tolerance);
    BOOST_CHECK_CLOSE(cell.getAlpha(),alpha,tolerance);
    BOOST_CHECK_CLOSE(cell.getBeta(),alpha,tolerance);
    BOOST_CHECK_CLOSE(cell.getGamma(),alpha,tolerance);

    BOOST_CHECK_CLOSE(cell.getVolume(),a*b*c,tolerance);

    const Eigen::Matrix3d& A=cell.getStandardM();
    BOOST_CHECK_CLOSE(A(0,0),a,tolerance);
    BOOST_CHECK_SMALL(A(1,0),tolerance);
    BOOST_CHECK_SMALL(A(2,0),tolerance);
    BOOST_CHECK_SMALL(A(0,1),tolerance);
    BOOST_CHECK_CLOSE(A(1,1),b,tolerance);
    BOOST_CHECK_SMALL(A(2,1),tolerance);
    BOOST_CHECK_SMALL(A(0,2),tolerance);
    BOOST_CHECK_SMALL(A(1,2),tolerance);
    BOOST_CHECK_CLOSE(A(2,2),c,tolerance);

    const Eigen::Matrix3d& B=cell.getReciprocalStandardM();
    BOOST_CHECK_CLOSE(B(0,0),1/a,tolerance);
    BOOST_CHECK_SMALL(B(1,0),tolerance);
    BOOST_CHECK_SMALL(B(2,0),tolerance);
    BOOST_CHECK_SMALL(B(0,1),tolerance);
    BOOST_CHECK_CLOSE(B(1,1),1/b,tolerance);
    BOOST_CHECK_SMALL(B(2,1),tolerance);
    BOOST_CHECK_SMALL(B(0,2),tolerance);
    BOOST_CHECK_SMALL(B(1,2),tolerance);
    BOOST_CHECK_CLOSE(B(2,2),1/c,tolerance);


    const Eigen::Matrix3d& G=cell.getMetricTensor();
    BOOST_CHECK_CLOSE(G(0,0),a*a,tolerance);
    BOOST_CHECK_SMALL(G(1,0),tolerance);
    BOOST_CHECK_SMALL(G(2,0),tolerance);
    BOOST_CHECK_SMALL(G(0,1),tolerance);
    BOOST_CHECK_CLOSE(G(1,1),b*b,tolerance);
    BOOST_CHECK_SMALL(G(2,1),tolerance);
    BOOST_CHECK_SMALL(G(0,2),tolerance);
    BOOST_CHECK_SMALL(G(1,2),tolerance);
    BOOST_CHECK_CLOSE(G(2,2),c*c,tolerance);

    cell.setLatticeCentring(LatticeCentring::I);
    cell.setBravaisType(BravaisType::Tetragonal);
    // Check angle calculations
    UnitCell cell4(10,10,10,90*deg,98*deg,90*deg);
    BOOST_CHECK_CLOSE(cell4.getAngle(1,0,0,0,0,1),82.0*deg,tolerance);

    // Check equivalence
    cell4.setSpaceGroup("P 4/m m m");

    BOOST_CHECK(cell4.isEquivalent(2,0,0,0,2,0));
    BOOST_CHECK(cell4.isEquivalent(2,3,2,3,2,-2));
    BOOST_CHECK(!cell4.isEquivalent(2,3,2,3,2,-3));

}
