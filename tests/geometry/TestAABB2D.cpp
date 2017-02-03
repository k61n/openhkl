#define BOOST_TEST_MODULE "Test AABB bounding box for 3D case"
#define BOOST_TEST_DYN_LINK

#include <cmath>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/AABB.h>

using namespace SX::Geometry;
using Eigen::Vector2d;
using Eigen::Vector3d;
const double tolerance=1e-5;


BOOST_AUTO_TEST_CASE(Test_AABB)
{
    AABB<double,2> bb;
    bb.setBounds(Vector2d(0,0),Vector2d(1,2));
    Eigen::Vector2d center, extends;
    center=bb.getAABBCenter();
    extends=bb.getAABBExtents();

    BOOST_CHECK_CLOSE(center[0], 0.5, tolerance);
    BOOST_CHECK_CLOSE(center[1], 1.0, tolerance);

    BOOST_CHECK_CLOSE(extends[0], 1, tolerance);
    BOOST_CHECK_CLOSE(extends[1], 2, tolerance);

    // Check the volume of the box
    BOOST_CHECK_CLOSE(bb.AABBVolume(),2,tolerance);
    // Check that a given point is inside
    BOOST_CHECK(bb.isInsideAABB(Vector2d(center)));
    // Check that a given point is outside
    BOOST_CHECK(!bb.isInsideAABB(Vector2d(2,3)));
    //A second bounding box, just touching
    AABB<double,2> bb2(Vector2d(1,2),Vector2d(2,3));
    BOOST_CHECK(bb2.intercept(bb));
    // Second bounding box overlaps .
    bb2.setBounds(Vector2d(0.5,2),Vector2d(2,3));
    BOOST_CHECK(bb2.intercept(bb));
    // No overlap
    bb2.setBounds(Vector2d(2,3),Vector2d(4,5));
    BOOST_CHECK(!(bb2.intercept(bb)));

    // Translate and scale operations
    bb.translateAABB(Vector2d(1,2));
    Vector2d& lower=bb.getLower();
    Vector2d& upper=bb.getUpper();
    BOOST_CHECK_CLOSE(lower[0], 1.0, tolerance);
    BOOST_CHECK_CLOSE(lower[1], 2.0, tolerance);
    BOOST_CHECK_CLOSE(upper[0], 2.0, tolerance);
    BOOST_CHECK_CLOSE(upper[1], 4.0, tolerance);
    bb.scaleAABB(Vector2d(1,2));
    lower=bb.getLower();
    upper=bb.getUpper();
    BOOST_CHECK_CLOSE(lower[0], 1.0, tolerance);
    BOOST_CHECK_CLOSE(lower[1], 1.0, tolerance);
    BOOST_CHECK_CLOSE(upper[0], 2.0, tolerance);
    BOOST_CHECK_CLOSE(upper[1], 5.0, tolerance);
    bb.scaleAABB(0.5);
    lower=bb.getLower();
    upper=bb.getUpper();
    BOOST_CHECK_CLOSE(lower[0], 1.25, tolerance);
    BOOST_CHECK_CLOSE(lower[1], 2.0, tolerance);
    BOOST_CHECK_CLOSE(upper[0], 1.75, tolerance);
    BOOST_CHECK_CLOSE(upper[1], 4.0, tolerance);

    // Test: (non-)intersection of the AABB with different rays

    AABB<double,2> bb2D;
    bb2D.setBounds(Vector2d(2,1),Vector2d(5,3));

    double t1,t2;
    BOOST_CHECK(!bb2D.rayIntersect(Vector2d(0,0),Vector2d(1,-1),t1,t2));
    BOOST_CHECK(bb2D.rayIntersect(Vector2d(0,0),Vector2d(1,1),t1,t2));
    BOOST_CHECK_CLOSE(t1, 2.0, 1.0e-10);
    BOOST_CHECK_CLOSE(t2, 3.0, 1.0e-10);
    BOOST_CHECK(bb2D.rayIntersect(Vector2d(0,0),Vector2d(-1,-1),t1,t2));
    BOOST_CHECK_CLOSE(t1,-3.0, 1.0e-10);
    BOOST_CHECK_CLOSE(t2,-2.0, 1.0e-10);
    BOOST_CHECK(bb2D.rayIntersect(Vector2d(3,2),Vector2d(1,1),t1,t2));
    BOOST_CHECK_CLOSE(t1,-1.0, 1.0e-10);
    BOOST_CHECK_CLOSE(t2, 1.0, 1.0e-10);
    BOOST_CHECK(bb2D.rayIntersect(Vector2d(-1,6),Vector2d(1.5,-1),t1,t2));
    BOOST_CHECK_CLOSE(t1, 3.0, 1.0e-10);
    BOOST_CHECK_CLOSE(t2, 4.0, 1.0e-10);
    BOOST_CHECK(bb2D.rayIntersect(Vector2d(0,1.000000000001),Vector2d(1,0),t1,t2));
    BOOST_CHECK_CLOSE(t1, 2.0, 1.0e-10);
    BOOST_CHECK_CLOSE(t2, 5.0, 1.0e-10);
    BOOST_CHECK(!bb2D.rayIntersect(Vector2d(0,0.9999999999999),Vector2d(1,0),t1,t2));
}
