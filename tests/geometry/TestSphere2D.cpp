#define BOOST_TEST_MODULE "Test Multi-dimensional Sphere"
#define BOOST_TEST_DYN_LINK
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>
#include "Sphere.h"

using namespace SX::Geometry;
using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Matrix2d;

const double tolerance=1e-5;
const double tolerance_large=1.0;

BOOST_AUTO_TEST_CASE(Test_Sphere)
{

	// Test: the construction of a 3D sphere
	Vector2d center(3,-2);
	double radius(10);
	Sphere<double,2> s1(center,radius);
	Vector2d lower(s1.getLower());
	Vector2d upper(s1.getUpper());

	BOOST_CHECK_CLOSE(lower[0], -7.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-12.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 13.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1],  8.0,tolerance);

	// Test: the isotropic scaling of a Sphere
	s1.scale(0.5);
	lower = s1.getLower();
	upper = s1.getUpper();
	BOOST_CHECK_CLOSE(lower[0],-2.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-7.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 8.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 3.0,tolerance);

	// Test: the translation of a Sphere
	s1.translate(Vector2d(3,1));
	lower = s1.getLower();
	upper = s1.getUpper();
	BOOST_CHECK_CLOSE(lower[0], 1.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-6.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0],11.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 4.0,tolerance);

	Vector3d point(0,0,1);
	std::cout<<s1.isInside(point)<<std::endl;

	// Test: a given point falls inside the Sphere
//	int nSteps(500);
//	Vector2d delta=(upper-lower)/nSteps;
//	Vector3d point(0,0,1);
//	double sum(0.0);
//	for(int i=0;i<=nSteps;++i)
//	{
//		point.x() = lower[0]+i*delta[0];
//		for(int j=0;j<=nSteps;++j)
//		{
//			point.y() = lower[1]+j*delta[1];
//			if (s1.isInside(point))
//				sum+=1.0;
//		}
//	}
//
//	sum *= ((upper[0]-lower[0])*(upper[1]-lower[1]))/(nSteps*nSteps);
//	BOOST_CHECK_CLOSE(sum,78.5,tolerance_large);
//
//	s1.translate(-s1.getCenter());
//	Sphere<double,2> s2(Vector2d(10,0),1.0);
//	BOOST_CHECK_EQUAL(s1.collide(s2),false);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),false);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),false);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),true);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),true);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),true);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),true);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),true);
//	s2.translate(Vector2d(-2,0));
//	BOOST_CHECK_EQUAL(s1.collide(s2),false);

}
