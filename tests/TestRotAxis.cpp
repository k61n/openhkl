#define BOOST_TEST_MODULE "Test Rotation Axis"
#define BOOST_TEST_DYN_LINK
#include "RotAxis.h"
#include "V3D.h"
#include "Matrix33.h"
#include "Units.h"
#include <cmath>
#include <boost/test/unit_test.hpp>

using namespace SX::Units;
using namespace SX::Geometry;

BOOST_AUTO_TEST_CASE(Test_Rot_Axis)
{
	//Empty Rotation axis initialize to (0,0,1)
    SX::Geometry::RotAxis a;
    V3D axis=a.getAxis();
    BOOST_CHECK(axis[0]==0);
    BOOST_CHECK(axis[1]==0);
    BOOST_CHECK(axis[2]==1);

    a.setRotationDirection(CW);
    SX::Matrix33<double> m=a.getMatrix(45.0*deg);
    BOOST_CHECK_CLOSE(m(0,0),0.5*sqrt(2.0),1e-6);
}
