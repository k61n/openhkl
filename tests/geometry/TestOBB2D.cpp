#define BOOST_TEST_MODULE "Test Multi-dimensional Object oriented Bounding Box"
#define BOOST_TEST_DYN_LINK
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>
#include "OBB.h"

using namespace SX::Geometry;
using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Vector4d;
using Eigen::Matrix2d;
using Eigen::Matrix3d;

const double tolerance=1e-5;
const double tolerance_large=1.0;

BOOST_AUTO_TEST_CASE(Test_OBB)
{

	// Test: the construction of an OBB
	Vector2d center(3,4);
	Vector2d extent(sqrt(2),sqrt(2));
	Matrix2d axis;
	axis << 1,0,
			0,1;
	OBB<double,2> obb1(center,extent,axis);
	Vector2d lower(obb1.getLower());
	Vector2d upper(obb1.getUpper());

	BOOST_CHECK_CLOSE(lower[0],3-sqrt(2),tolerance);
	BOOST_CHECK_CLOSE(lower[1],4-sqrt(2),tolerance);
	BOOST_CHECK_CLOSE(upper[0],3+sqrt(2),tolerance);
	BOOST_CHECK_CLOSE(upper[1],4+sqrt(2),tolerance);

	// Test: the rotation of an OBB
	Matrix2d r;
	r << 1,-1,
		 1, 1;
	obb1.rotate(r);
	lower=obb1.getLower();
	upper=obb1.getUpper();
	BOOST_CHECK_CLOSE(lower[0],1.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],2.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0],5.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1],6.0,tolerance);

	// Test: the isotropic scaling of an OBB
	center << 3,2;
	extent << 1,4;
	axis << 1, 0,
			0, 1;
	OBB<double,2> obb2(center,extent,axis);
	obb2.scale(5);
	lower = obb2.getLower();
	upper = obb2.getUpper();
	BOOST_CHECK_CLOSE(lower[0], -2.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-18.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 8.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1],22.0,tolerance);

	// Test: the anisotropic scaling of an OBB
	center << 2,2;
	extent << sqrt(2)/2,sqrt(2);
	axis << 1,-1,
			1, 1;
	OBB<double,2> obb3(center,extent,axis);
	obb3.scale(Vector2d(3,2));
	lower = obb3.getLower();
	upper = obb3.getUpper();
	BOOST_CHECK_CLOSE(lower[0],-1.5,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-1.5,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 5.5,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 5.5,tolerance);

	// Test: the translation of an OBB
	obb3.translate(Vector2d(-1,2));
	lower = obb3.getLower();
	upper = obb3.getUpper();
	BOOST_CHECK_CLOSE(lower[0],-2.5,tolerance);
	BOOST_CHECK_CLOSE(lower[1], 0.5,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 4.5,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 7.5,tolerance);

	// Test: a given point falls inside the OBB

	obb3.translate(Vector2d(1,-2));
	lower = obb3.getLower();
	upper = obb3.getUpper();

	BOOST_CHECK_CLOSE(lower[0],-1.5,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-1.5,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 5.5,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 5.5,tolerance);

	int nSteps(500);
	Vector2d delta=(upper-lower)/nSteps;
	Vector3d point(0,0,1);
	double sum(0.0);
	for(int i=0;i<=nSteps;++i)
	{
		point.x() = lower[0]+i*delta[0];
		for(int j=0;j<=nSteps;++j)
		{
			point.y() = lower[1]+j*delta[1];
			if (obb3.isInside(point))
				sum+=1.0;
		}
	}

	sum *= ((upper[0]-lower[0])*(upper[1]-lower[1]))/(nSteps*nSteps);

	BOOST_CHECK_CLOSE(sum,24,tolerance_large);

	// Test: the (non-)intersection of an OBB and different rays
	axis << 1,0,
			0,1;
	OBB<double,2> obb4(Vector2d(1,1),Vector2d(2,2),axis);

	double t1,t2;
	BOOST_CHECK(obb4.rayIntersect(Vector2d(-5,0),Vector2d(2,0.5),t1,t2));
	BOOST_CHECK_CLOSE(t1, 2.0, 1.0e-10);
	BOOST_CHECK_CLOSE(t2, 4.0, 1.0e-10);
	BOOST_CHECK(obb4.rayIntersect(Vector2d(-3,3),Vector2d(4,-2),t1,t2));
	BOOST_CHECK_CLOSE(t1, 0.5, 1.0e-10);
	BOOST_CHECK_CLOSE(t2, 1.5, 1.0e-10);
	BOOST_CHECK(!obb4.rayIntersect(Vector2d(-6,8),Vector2d(2,3),t1,t2));

	Vector2d s(sqrt(2),sqrt(2));
	obb4.scale(s);

	r << sqrt(2.0),-sqrt(2.0),
		 sqrt(2.0), sqrt(2.0);
	obb4.rotate(r);

	BOOST_CHECK(obb4.rayIntersect(Vector2d(-4,-2),Vector2d(1.5,0.5),t1,t2));
	BOOST_CHECK_CLOSE(t1, 2.0, 1.0e-10);
	BOOST_CHECK_CLOSE(t2, 6.0, 1.0e-10);
	BOOST_CHECK(obb4.rayIntersect(Vector2d(5,2.5),Vector2d(-0.5,0),t1,t2));
	BOOST_CHECK_CLOSE(t1, 3.0, 1.0e-10);
	BOOST_CHECK_CLOSE(t2, 13.0, 1.0e-10);
	BOOST_CHECK(obb4.rayIntersect(Vector2d(5,2.5),Vector2d(0.5,0),t1,t2));
	BOOST_CHECK_CLOSE(t1, -13.0, 1.0e-10);
	BOOST_CHECK_CLOSE(t2,  -3.0, 1.0e-10);
	BOOST_CHECK(!obb4.rayIntersect(Vector2d(-6,8),Vector2d(2,5),t1,t2));
}
