#define BOOST_TEST_MODULE "Test Multi-dimensional Ellipsoid"
#define BOOST_TEST_DYN_LINK
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>
#include "OBB.h"

using namespace SX::Geometry;
using Eigen::Vector3d;
using Eigen::Matrix3d;

const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_OBB)
{
	Vector3d center(3,2,4);
	Vector3d semi_axes(sqrt(2),sqrt(2),1);
	Matrix3d eigV;
	eigV << 1, -1 ,0,
			1,  1 ,0,
			0,  0 ,1;
	OBB<double,3> e(center,semi_axes,eigV);

	Vector3d lower(e.getLower());
	Vector3d upper(e.getUpper());

	BOOST_CHECK_CLOSE(lower[0],1.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],0.0,tolerance);
	BOOST_CHECK_CLOSE(lower[2],3.0,tolerance);

	BOOST_CHECK_CLOSE(upper[0],5.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1],4.0,tolerance);
	BOOST_CHECK_CLOSE(upper[2],5.0,tolerance);

}
