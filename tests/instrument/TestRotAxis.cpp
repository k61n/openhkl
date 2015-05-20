#define BOOST_TEST_MODULE "Test Rotation Axis"
#define BOOST_TEST_DYN_LINK
#include "RotAxis.h"
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include <fstream>


using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_Rot_Axis)
{
	//Empty Rotation axis initialize to (0,0,1)
    SX::Instrument::RotAxis a("omega",Vector3d(0,0,1));
    Vector3d axis=a.getAxis();
    BOOST_CHECK(axis[0]==0);
    BOOST_CHECK(axis[1]==0);
    BOOST_CHECK(axis[2]==1);
    BOOST_CHECK_CLOSE(a.getOffset(),0.0,tolerance);
    a.setRotationDirection(RotAxis::CCW);
    // Check that a rotation CCW of 45.0 degrees brings the 1,0,0 vector into 1/2.(sqrt(2),sqrt(2),0)
    Vector3d transf=a.transform(Vector3d(1,0,0),45.0*deg);
    BOOST_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
    BOOST_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
    BOOST_CHECK_CLOSE(transf[2],0.0,tolerance);
    // Check same for CCW of 0,1,0
    transf=a.transform(Vector3d(0,1,0),45.0*deg);
	BOOST_CHECK_CLOSE(transf[0],-0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[2],0.0,tolerance);

	// Switch rotation direction CW
	a.setRotationDirection(RotAxis::CW);
	transf=a.transform(Vector3d(1,0,0),45.0*deg);
	BOOST_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[1],-0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[2],0.0,tolerance);
	// Check same for CW of 0,1,0
	transf=a.transform(Vector3d(0,1,0),45.0*deg);
	BOOST_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[2],0.0,tolerance);

	// Ensure that offset is taken into account
	a.setRotationDirection(RotAxis::CCW);
	a.setOffset(1.0*deg);
	transf=a.transform(Vector3d(1,0,0),44.0*deg);
	BOOST_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[2],0.0,tolerance);

	// Switch rotation axis to y
	a.setOffset(0.0);
	a.setAxis(Vector3d(0,1,0));
	transf=a.transform(Vector3d(1,0,0),45.0*deg);
	BOOST_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[1],0.0,tolerance);
	BOOST_CHECK_CLOSE(transf[2],-0.5*sqrt(2.0),tolerance);

	// Test with Homogeneous matrix
	transf=a.transform(Vector3d(1,0,0),45.0*deg);
	BOOST_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
	BOOST_CHECK_CLOSE(transf[1],0.0,tolerance);
	BOOST_CHECK_CLOSE(transf[2],-0.5*sqrt(2.0),tolerance);

}
