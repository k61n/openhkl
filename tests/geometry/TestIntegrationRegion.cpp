#define BOOST_TEST_MODULE "Test Integration Region"
#define BOOST_TEST_DYN_LINK

#include <cmath>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/Ellipsoid.h>
#include <nsxlib/geometry/IntegrationRegion.h>
#include <nsxlib/geometry/ShapeUnion.h>

using namespace nsx::Geometry;
using Eigen::Vector2d;
using Eigen::Vector3d;
const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_IntegrationRegion)
{

}
