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

	// Test: the construction onf an OBB
	Vector3d center(3,3,4);
	Vector3d semi_axes(sqrt(2),sqrt(2),1);
	Matrix3d eigen_values;
	eigen_values << 1,-1 ,0,
			        1, 1 ,0,
			        0, 0 ,1;
	OBB<double,3> obb1(center,semi_axes,eigen_values);
	Vector3d lower(obb1.getLower());
	Vector3d upper(obb1.getUpper());

	BOOST_CHECK_CLOSE(lower[0],1.0,tolerance);
	BOOST_CHECK_CLOSE(lower[1],1.0,tolerance);
	BOOST_CHECK_CLOSE(lower[2],3.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0],5.0,tolerance);
	BOOST_CHECK_CLOSE(upper[1],5.0,tolerance);
	BOOST_CHECK_CLOSE(upper[2],5.0,tolerance);

	// Test: the isotropic scaling of an OBB
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

	// Test: the anisotropic scaling of an OBB
	center << 2,2,1;
	semi_axes << sqrt(2)/2,sqrt(2),2;
	eigen_values << 1,-1 ,0,
			        1, 1 ,0,
			        0, 0 ,1;
	OBB<double,3> obb3(center,semi_axes,eigen_values);
	obb3.scale(Vector3d(3,2,5));
	lower = obb3.getLower();
	upper = obb3.getUpper();
	BOOST_CHECK_CLOSE(lower[0],-1.5,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-1.5,tolerance);
	BOOST_CHECK_CLOSE(lower[2],-9.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 5.5,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 5.5,tolerance);
	BOOST_CHECK_CLOSE(upper[2],11.0,tolerance);

	// Test: the translation of an OBB
	obb3.translate(Vector3d(-1,2,4));
	lower = obb3.getLower();
	upper = obb3.getUpper();
	BOOST_CHECK_CLOSE(lower[0],-2.5,tolerance);
	BOOST_CHECK_CLOSE(lower[1], 0.5,tolerance);
	BOOST_CHECK_CLOSE(lower[2],-5.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 4.5,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 7.5,tolerance);
	BOOST_CHECK_CLOSE(upper[2],15.0,tolerance);

	// Test: a given point falls inside the OBB

	obb3.translate(Vector3d(1,-2,-4));
	lower = obb3.getLower();
	upper = obb3.getUpper();

	BOOST_CHECK_CLOSE(lower[0],-1.5,tolerance);
	BOOST_CHECK_CLOSE(lower[1],-1.5,tolerance);
	BOOST_CHECK_CLOSE(lower[2],-9.0,tolerance);
	BOOST_CHECK_CLOSE(upper[0], 5.5,tolerance);
	BOOST_CHECK_CLOSE(upper[1], 5.5,tolerance);
	BOOST_CHECK_CLOSE(upper[2],11.0,tolerance);


	int nSteps(500);
	Vector3d delta=(upper-lower)/nSteps;
	Vector4d point(0,0,0,1);
	double sum(0.0);
	for(int i=0;i<=nSteps;++i)
	{
		point.x() = lower[0]+i*delta[0];
		for(int j=0;j<=nSteps;++j)
		{
			point.y() = lower[1]+j*delta[1];
			for(int k=0;k<=nSteps;++k)
			{
				point.z() = lower[2]+k*delta[2];
				if (obb3.isInside(point))
					sum+=1.0;
			}
		}
	}

	sum *= ((upper[0]-lower[0])*(upper[1]-lower[1])*(upper[2]-lower[2]))/(nSteps*nSteps*nSteps);

	BOOST_CHECK_CLOSE(sum,480,tolerance_large);


}
