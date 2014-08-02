#define BOOST_TEST_MODULE "Test Translation Axis"
#define BOOST_TEST_DYN_LINK
#include "TransAxis.h"
#include "Units.h"
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include <fstream>


using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_Trans_Axis)
{

		TransAxis t("x",Vector3d(1,0,0));
		// Translate the vector 0,1,2 by 0.4
		Vector3d v=t.transform(Vector3d(0,1,2),0.4);
		BOOST_CHECK_CLOSE(v[0],0.4,tolerance);
		BOOST_CHECK_CLOSE(v[1],1.0,tolerance);
		BOOST_CHECK_CLOSE(v[2],2.0,tolerance);


}
