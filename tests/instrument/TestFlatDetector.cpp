#define BOOST_TEST_MODULE "Test Flat Detector"
#define BOOST_TEST_DYN_LINK
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include "FlatDetector.h"
#include "Gonio.h"
#include <iostream>

using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
// 0.1% error
const double tolerance=1e-3;
BOOST_AUTO_TEST_CASE(Test_Flat_Detector)
{
	FlatDetector d("D10-detector");
	d.setDistance(380*mm);
	d.setDimensions(80*mm,80*mm);
	d.setNPixels(32,32);

	// This should be the center of the detector at rest at (0,0.764,0)
	Eigen::Vector3d center=d.getEventPosition(15.5,15.5);
	BOOST_CHECK_SMALL(center[0],tolerance);
	BOOST_CHECK_CLOSE(center[1],0.380,tolerance);
	BOOST_CHECK_SMALL(center[2],tolerance);

	double gamma,nu;
	d.getGammaNu(15.5,15.5,gamma,nu);
	BOOST_CHECK_SMALL(gamma,tolerance);
	BOOST_CHECK_SMALL(nu,tolerance);
	double th2=d.get2Theta(15.5,15.5);
	BOOST_CHECK_SMALL(th2,tolerance);

	// Attach a gonio
	std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
	g->addRotation("gamma",Vector3d(0,0,1),RotAxis::CW);
	d.setGonio(g);

	center=d.getEventPosition(15.5,15.5,{90.0*deg});
	BOOST_CHECK_CLOSE(center[0],0.380,tolerance);
	BOOST_CHECK_SMALL(center[1],0.001);
	BOOST_CHECK_SMALL(center[2],0.001);
	d.getGammaNu(15.5,15.5,gamma,nu,{90.0*deg});
	BOOST_CHECK_CLOSE(gamma,90*deg,tolerance);
	BOOST_CHECK_SMALL(nu,0.001);

	th2=d.get2Theta(15.5,15.5,{90.0*deg});
	BOOST_CHECK_CLOSE(th2,90.0*deg,tolerance);
	// Scattering in the center of the detector with wavelength 2.0
	// should get kf = (0.5,0,0)
	Eigen::Vector3d kf=d.getKf(15.5,15.5,2.0,{90.0*deg});
	BOOST_CHECK_CLOSE(kf[0],0.5,tolerance);
	BOOST_CHECK_SMALL(kf[1],0.001);
	BOOST_CHECK_SMALL(kf[2],0.001);
	// Should be 45 deg in the x,-y plane
	Eigen::Vector3d Q=d.getQ(15.5,15.5,2.0,{90.0*deg});
	BOOST_CHECK_CLOSE(Q[0],0.5,tolerance);
	BOOST_CHECK_CLOSE(Q[1],-0.5,tolerance);
	BOOST_CHECK_SMALL(Q[2],0.001);




}

