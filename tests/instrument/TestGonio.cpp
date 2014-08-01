#define BOOST_TEST_MODULE "Test Gonio"
#define BOOST_TEST_DYN_LINK
#include "Gonio.h"
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include "Axis.h"

using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_Gonio)
{

	// A simple gonio as defined by Busing-Levi
	Gonio t("Translation table");
	t.addTranslation("x",Vector3d(1,0,0));
	t.addTranslation("y",Vector3d(0,1,0));
	t.addTranslation("z",Vector3d(0,0,1));
	// Transform (0,0,0)
	Vector3d result=t.transform({1*m,2*m,3*m},Vector3d(0,0,0));
	BOOST_CHECK_CLOSE(result[0],1,tolerance);
	BOOST_CHECK_CLOSE(result[1],2,tolerance);
	BOOST_CHECK_CLOSE(result[2],3,tolerance);

}
