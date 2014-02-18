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
	Matrix3d eigen_values;
	eigen_values << 1,-1 ,0,
			        1, 1 ,0,
			        0, 0 ,1;
	OBB<double,3> obb1(center,semi_axes,eigen_values);

	Vector3d lower(obb1.getLower());
	Vector3d upper(obb1.getUpper());

	BOOST_CHECK_CLOSE(lower[0],1.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],0.0,tolerance);
	BOOST_CHECK_CLOSE(lower[2],3.0,tolerance);

	BOOST_CHECK_CLOSE(upper[0],5.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1],4.0,tolerance);
	BOOST_CHECK_CLOSE(upper[2],5.0,tolerance);

	center << 3,2,4;
	semi_axes << 1,4,2;
	eigen_values << 1, 0 ,0,
			        0, 1 ,0,
			        0, 0 ,1;

	OBB<double,3> obb2(center,semi_axes,eigen_values);
	obb2.scale(5);

	lower = obb2.getLower();
	upper = obb2.getUpper();

	BOOST_CHECK_CLOSE(lower[0], -2.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-18.0,tolerance);
	BOOST_CHECK_CLOSE(lower[2], -6.0,tolerance);

	BOOST_CHECK_CLOSE(upper[0], 8.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1],22.0,tolerance);
	BOOST_CHECK_CLOSE(upper[2],14.0,tolerance);


}
