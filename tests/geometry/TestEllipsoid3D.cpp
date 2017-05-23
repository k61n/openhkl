#define BOOST_TEST_MODULE "Test Ellipsoid 3D"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/Ellipsoid.h>

using Eigen::Vector3d;
using Eigen::Matrix3d;

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_Ellipsoid_3D)
{
    Vector3d center(10,10,10);
    Vector3d semi_axes(3,3,4);
    Matrix3d eigV;
    eigV << 1,0,0,
            0,1,0,
            0,0,1;
    Ellipsoid e(center,semi_axes,eigV);
    e.translate(Vector3d(1,0,0));
}
