#define BOOST_TEST_MODULE "Test Multi-dimensional Ellipsoid"
#define BOOST_TEST_DYN_LINK
#include "Ellipsoid.h"
#include <cmath>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>

using namespace SX::Geometry;
using Eigen::Vector3d;
using Eigen::Vector4d;
using Eigen::Matrix3d;
using Eigen::Vector2d;
using Eigen::Matrix2d;
// const double tolerance=1e-5;


BOOST_AUTO_TEST_CASE(Test_NDEllipsoid)
{
    Vector3d center(10,10,10);
    Vector3d semi_axes(3,3,4);
    Matrix3d eigV;
    eigV << 1,0,0,
            0,1,0,
            0,0,1;
    Ellipsoid<double,3> e(center,semi_axes,eigV);
    e.translate(Vector3d(1,0,0));
}
