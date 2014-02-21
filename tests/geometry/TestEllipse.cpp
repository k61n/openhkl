#define BOOST_TEST_MODULE "Test 2-dimensional Ellipsoid"
#define BOOST_TEST_DYN_LINK
#include "NDEllipsoid.h"
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>

using namespace SX::Geometry;
using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Matrix2d;

const double tolerance=1e-5;


BOOST_AUTO_TEST_CASE(Test_NDEllipsoid)
{
	Vector2d center1(300,300);
	Vector2d center2(400,400);
	Vector2d semi_axes(20,35);
	Matrix2d eigV;
	eigV << 1,0,
			0,1;
	NDEllipsoid<double,2> e1(center1,semi_axes,eigV);
	NDEllipsoid<double,2> e2(center2,semi_axes,eigV);
	std::cout << " I am here" << e1.collide(e2) << std::endl;
}
