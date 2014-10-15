#define BOOST_TEST_MODULE "Test Gonio"
#define BOOST_TEST_DYN_LINK
#include "Gonio.h"
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include "Axis.h"

using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
const double tolerance=1e-6;


BOOST_AUTO_TEST_CASE(Test_Gonio)
{

	// A simple translation table
	Gonio t("Translation table");
	t.addTranslation("x",Vector3d(1,0,0));
	t.addTranslation("y",Vector3d(0,1,0));
	t.addTranslation("z",Vector3d(0,0,1));

	t.getAxis("x")->setPhysical(true);
	t.getAxis("y")->setPhysical(true);
	t.getAxis("z")->setPhysical(true);

	// Transform (0,0,0)
	Vector3d result=t.transform(Vector3d(0,0,0),{1,2,3});
	BOOST_CHECK_CLOSE(result[0],1,tolerance);
	BOOST_CHECK_CLOSE(result[1],2,tolerance);
	BOOST_CHECK_CLOSE(result[2],3,tolerance);
	// Check that throws if the number of parameters is invalid
	BOOST_CHECK_THROW(t.transform(Vector3d(0,0,0),{1,2}),std::range_error);
	const Axis* a0=t.getAxis(0);
	const Axis* a1=t.getAxis(1);
	const Axis* a2=t.getAxis(2);
	BOOST_CHECK_EQUAL(a0->getLabel(),"x");
	BOOST_CHECK_EQUAL(a1->getLabel(),"y");
	BOOST_CHECK_EQUAL(a2->getLabel(),"z");

	// Simple goniometer as in Busing Levy
	Gonio g("Busing Levy convention");
	g.addRotation("omega",Vector3d(0,0,1),RotAxis::CW);
	g.addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
	g.addRotation("phi",Vector3d(0,0,1),RotAxis::CW);


	// Check that result of combined rotation is Ok.
	double om=12*deg;
	double chi=24*deg;
	double phi=55*deg;
	Matrix3d OM;
	OM << cos(om), sin(om), 0,
		 -sin(om), cos(om), 0,
		  0      ,  0      , 1;
	Matrix3d CH;
	CH << cos(chi), 0, sin(chi),
		  0 ,       1,      0,
		 -sin(chi), 0, cos(chi);
	Matrix3d PH;
	PH << cos(phi), sin(phi), 0,
		 -sin(phi), cos(phi), 0,
		  0      ,  0      , 1;

	Vector3d result1=OM*CH*PH*Vector3d(1,0,0);

	result=g.transform(Vector3d(1,0,0),{om,chi,phi});
	BOOST_CHECK_CLOSE(result[0],result1[0],tolerance);
	BOOST_CHECK_CLOSE(result[1],result1[1],tolerance);
	BOOST_CHECK_CLOSE(result[2],result1[2],tolerance);


}
