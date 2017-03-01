#define BOOST_TEST_MODULE "Test Shape Union"
#define BOOST_TEST_DYN_LINK

#include <cmath>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/AABB.h>
#include <nsxlib/geometry/ShapeUnion.h>

using namespace SX::Geometry;
using Eigen::Vector2d;
using Eigen::Vector3d;
const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_ShapeUnion)
{
    ShapeUnion<double, 3> shape;
    AABB<double,3> bb1, bb2;
    bb1.setBounds(Vector3d(0,0,0),Vector3d(1,2,3));
    bb2.setBounds(Vector3d(0,1,2),Vector3d(3,4,5));

    shape.addShape(bb1);
    shape.addShape(bb2);
}
