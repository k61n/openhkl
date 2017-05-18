#define BOOST_TEST_MODULE "Test Ellipsoid 2D"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/Ellipsoid.h>

using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Matrix2d;

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_Ellipsoid_2D)
{
    Vector2d center1(300,300);
    Vector2d center2(400,400);
    Vector2d semi_axes(20,35);
    Matrix2d eigV;
    eigV << 1,0,
            0,1;
    Ellipsoid<double,2> e1(center1,semi_axes,eigV);
    Ellipsoid<double,2> e2(center2,semi_axes,eigV);

    BOOST_CHECK(!e1.intercept(e2));

    // Test: the (non-)intersection of an Sphere and different rays
    double t1,t2;
    BOOST_CHECK(e1.rayIntersect(Vector2d(300,260),Vector2d(0,1.0),t1,t2));
    BOOST_CHECK_CLOSE(t1, 5.0, 1.0e-10);
    BOOST_CHECK_CLOSE(t2,75.0, 1.0e-10);

}
