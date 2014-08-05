#define BOOST_TEST_MODULE "Test Cylindrical Detector"
#define BOOST_TEST_DYN_LINK
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include "CylindricalDetector.h"
#include "Gonio.h"
#include <iostream>

using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_Cylindrical_Detector)
{
	CylindricalDetector d("D19-detector");
	d.setDistance(764*mm);
	d.setWidthAngle(120.0*deg);
	d.setHeight(40.0*cm);
	d.setNPixels(640,256);
	std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
	g->addRotation("gamma",Vector3d(0,0,1),RotAxis::CW);


}

