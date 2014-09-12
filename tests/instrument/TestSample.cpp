#define BOOST_TEST_MODULE "Test Sample"
#define BOOST_TEST_DYN_LINK
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include "Sample.h"
#include "Gonio.h"
#include <iostream>

using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
// 0.1% error
const double tolerance=1e-3;
BOOST_AUTO_TEST_CASE(Test_Sample)
{

	Sample s("sample");

	// Simple goniometer as in Busing Levy
	std::shared_ptr<Gonio> g(new Gonio("BL-convention"));
	g->addRotation("omega",Vector3d(0,0,1),RotAxis::CW);
	g->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
	g->addRotation("phi",Vector3d(0,0,1),RotAxis::CW);
	// Attach a gonio

	s.setGonio(g);





}

