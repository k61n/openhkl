#define BOOST_TEST_MODULE "Test AABB bounding box"
#define BOOST_TEST_DYN_LINK
#include "AABB.h"
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>

using namespace SX::Geometry;

const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_AABB)
{
	AABB<double,3> bb;
	Eigen::Vector3d min, max;
	min << 0,0,0;
	max << 1,2,3;
	bb.setBounds(min,max);
	Eigen::Vector3d center, extends;
	center=bb.getCenter();
	extends=bb.getExtents();
	BOOST_CHECK_CLOSE(center[0], 0.5, tolerance);
	BOOST_CHECK_CLOSE(center[1], 1.0, tolerance);
	BOOST_CHECK_CLOSE(center[2], 1.5, tolerance);

	BOOST_CHECK_CLOSE(extends[0], 1, tolerance);
	BOOST_CHECK_CLOSE(extends[1], 2, tolerance);
	BOOST_CHECK_CLOSE(extends[2], 3, tolerance);
}
