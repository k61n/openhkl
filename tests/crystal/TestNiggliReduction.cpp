#define BOOST_TEST_MODULE "Test Niggli Reduction"
#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/NiggliReduction.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Niggli_Reduction)
{
    // An oblique cell representing an orthorhombic 2,1,3 cell
    // o-----o-----o
    // |     |     |
    // o-----o-----o
    nsx::UnitCell cell(2.0,sqrt(17.0),3.0,90*nsx::deg,90*nsx::deg,std::atan(1.0/4));
    const Eigen::Matrix3d& g=cell.getMetricTensor();

    nsx::NiggliReduction n(g,1e-3);
    Eigen::Matrix3d gprime,P;
    n.reduce(gprime,P);

    // Check that the Unit Cell is 1 , 2 , 3
    cell.transform(P);
    BOOST_CHECK_CLOSE(cell.getA(),1.0,tolerance);
    BOOST_CHECK_CLOSE(cell.getB(),2.0,tolerance);
    BOOST_CHECK_CLOSE(cell.getC(),3.0,tolerance);
    BOOST_CHECK_CLOSE(cell.getAlpha(),90*nsx::deg,tolerance);
    BOOST_CHECK_CLOSE(cell.getBeta(),90*nsx::deg,tolerance);
    BOOST_CHECK_CLOSE(cell.getGamma(),90*nsx::deg,tolerance);
}
