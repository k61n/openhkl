#define BOOST_TEST_MODULE "Test AABB bounding box for 3D case"
#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/AABB.h>

const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_AABB3D)
{
    nsx::AABB bb;
    bb.setBounds(Eigen::Vector3d(0,0,0),Eigen::Vector3d(1,2,3));
    Eigen::Vector3d center, extends;
    center=bb.center();
    extends=bb.extents();

    BOOST_CHECK_CLOSE(center[0], 0.5, tolerance);
    BOOST_CHECK_CLOSE(center[1], 1.0, tolerance);
    BOOST_CHECK_CLOSE(center[2], 1.5, tolerance);

    BOOST_CHECK_CLOSE(extends[0], 1, tolerance);
    BOOST_CHECK_CLOSE(extends[1], 2, tolerance);
    BOOST_CHECK_CLOSE(extends[2], 3, tolerance);
    // Check that a given point is inside
    BOOST_CHECK(bb.isInside(Eigen::Vector3d(center)));
    // Check that a given point is outside
    BOOST_CHECK(!bb.isInside(Eigen::Vector3d(2,3,4)));
    //A second bounding box, just touching
    nsx::AABB bb2(Eigen::Vector3d(1,2,3),Eigen::Vector3d(2,3,4));
    BOOST_CHECK(bb2.collide(bb));
    // Second bounding box overlaps .
    bb2.setBounds(Eigen::Vector3d(0.5,2,3),Eigen::Vector3d(2,3,4));
    BOOST_CHECK(bb2.collide(bb));
    // No overlap
    bb2.setBounds(Eigen::Vector3d(2,3,4),Eigen::Vector3d(4,5,6));
    BOOST_CHECK(!(bb2.collide(bb)));

    // Translate and scale operations
    bb.translateAABB(Eigen::Vector3d(1,2,3));
    Eigen::Vector3d lower=bb.lower();
    Eigen::Vector3d upper=bb.upper();
    BOOST_CHECK_CLOSE(lower[0], 1.0, tolerance);
    BOOST_CHECK_CLOSE(lower[1], 2.0, tolerance);
    BOOST_CHECK_CLOSE(lower[2], 3.0, tolerance);
    BOOST_CHECK_CLOSE(upper[0], 2.0, tolerance);
    BOOST_CHECK_CLOSE(upper[1], 4.0, tolerance);
    BOOST_CHECK_CLOSE(upper[2], 6.0, tolerance);
    bb.scaleAABB(Eigen::Vector3d(1,2,3));
    lower=bb.lower();
    upper=bb.upper();
    BOOST_CHECK_CLOSE(lower[0], 1.0, tolerance);
    BOOST_CHECK_CLOSE(lower[1], 1.0, tolerance);
    BOOST_CHECK_CLOSE(lower[2], 0.0, tolerance);
    BOOST_CHECK_CLOSE(upper[0], 2.0, tolerance);
    BOOST_CHECK_CLOSE(upper[1], 5.0, tolerance);
    BOOST_CHECK_CLOSE(upper[2], 9.0, tolerance);
    bb.scaleAABB(0.5);
    lower=bb.lower();
    upper=bb.upper();
    BOOST_CHECK_CLOSE(lower[0], 1.25, tolerance);
    BOOST_CHECK_CLOSE(lower[1], 2.0, tolerance);
    BOOST_CHECK_CLOSE(lower[2], 2.25, tolerance);
    BOOST_CHECK_CLOSE(upper[0], 1.75, tolerance);
    BOOST_CHECK_CLOSE(upper[1], 4.0, tolerance);
    BOOST_CHECK_CLOSE(upper[2], 6.75, tolerance);

    // Test: (non-)intersection of the AABB with different rays

    double t1,t2;
    nsx::AABB bb3D;
    bb3D.setBounds(Eigen::Vector3d(1,1,1),Eigen::Vector3d(3,3,3));
    BOOST_CHECK(bb3D.rayIntersect(Eigen::Vector3d(-1,-1,-1),Eigen::Vector3d(1,1,1),t1,t2));
    BOOST_CHECK_CLOSE(t1, 2.0, 1.0e-10);
    BOOST_CHECK_CLOSE(t2, 4.0, 1.0e-10);
}
