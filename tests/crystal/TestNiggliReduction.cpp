#define BOOST_TEST_MODULE "Test Niggli Reduction"
#define BOOST_TEST_DYN_LINK
#include "UnitCell.h"
#include "NiggliReduction.h"
#include <boost/test/unit_test.hpp>
#include <string>
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <iostream>

using namespace SX::Crystal;
using namespace SX::Units;
const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_Niggli_Reduction)
{
	// An oblique cell representing an orthorhombic 2,1,3 cell
	// o-----o-----o
	// |     |     |
	// o-----o-----o
	UnitCell cell(2.0,sqrt(17.0),3.0,90*deg,90*deg,atan(1.0/4));
	const Eigen::Matrix3d& g=cell.getMetricTensor();

	NiggliReduction n(g,1e-3);
	Eigen::Matrix3d gprime,P;
	n.reduce(gprime,P);

	// Check that the Unit Cell is 1 , 2 , 3
	cell.setMetricTensor(gprime);
	BOOST_CHECK_CLOSE(cell.getA(),1.0,tolerance);
	BOOST_CHECK_CLOSE(cell.getB(),2.0,tolerance);
	BOOST_CHECK_CLOSE(cell.getC(),3.0,tolerance);
	BOOST_CHECK_CLOSE(cell.getAlpha(),90*deg,tolerance);
	BOOST_CHECK_CLOSE(cell.getBeta(),90*deg,tolerance);
	BOOST_CHECK_CLOSE(cell.getGamma(),90*deg,tolerance);

	std::cout << P <<std::endl;
}
