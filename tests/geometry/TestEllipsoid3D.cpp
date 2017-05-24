#define BOOST_TEST_MODULE "Test Ellipsoid 3D"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/Ellipsoid.h>

const double eps = 1e-8;

BOOST_AUTO_TEST_CASE(Test_Ellipsoid_3D)
{
    Eigen::Vector3d center(10,10,10);
    Eigen::Vector3d semi_axes(3,3,4);
    Eigen::Matrix3d eigV;
    eigV << 1,0,0,
            0,1,0,
            0,0,1;
    nsx::Ellipsoid e(center,semi_axes,eigV);

    auto p = e.getAABBCenter();

    BOOST_CHECK_CLOSE(p(0), center(0), eps);
    BOOST_CHECK_CLOSE(p(1), center(1), eps);
    BOOST_CHECK_CLOSE(p(2), center(2), eps);

    auto lower = e.getLower();

    BOOST_CHECK_CLOSE(lower(0), -3+center(0), eps);
    BOOST_CHECK_CLOSE(lower(1), -3+center(1), eps);
    BOOST_CHECK_CLOSE(lower(2), -4+center(2), eps);

    auto upper = e.getUpper();

    BOOST_CHECK_CLOSE(upper(0), 3+center(0), eps);
    BOOST_CHECK_CLOSE(upper(1), 3+center(1), eps);
    BOOST_CHECK_CLOSE(upper(2), 4+center(2), eps);

    auto extents = e.getAABBExtents();

    BOOST_CHECK_CLOSE(extents(0), 6, eps);
    BOOST_CHECK_CLOSE(extents(1), 6, eps);
    BOOST_CHECK_CLOSE(extents(2), 8, eps);

    BOOST_CHECK_CLOSE(e.getVolume(), 3*3*4 * 4 * M_PI / 3.0, eps);

    Eigen::Vector3d shift(1,-1,2);

    e.translate(shift);

    p = e.getAABBCenter();

    BOOST_CHECK_CLOSE(p(0), center(0)+shift(0), eps);
    BOOST_CHECK_CLOSE(p(1), center(1)+shift(1), eps);
    BOOST_CHECK_CLOSE(p(2), center(2)+shift(2), eps);

    lower = e.getLower();

    BOOST_CHECK_CLOSE(lower(0), -3+center(0)+shift(0), eps);
    BOOST_CHECK_CLOSE(lower(1), -3+center(1)+shift(1), eps);
    BOOST_CHECK_CLOSE(lower(2), -4+center(2)+shift(2), eps);

    upper = e.getUpper();

    BOOST_CHECK_CLOSE(upper(0), 3+center(0)+shift(0), eps);
    BOOST_CHECK_CLOSE(upper(1), 3+center(1)+shift(1), eps);
    BOOST_CHECK_CLOSE(upper(2), 4+center(2)+shift(2), eps);

    extents = e.getAABBExtents();

    BOOST_CHECK_CLOSE(extents(0), 6, eps);
    BOOST_CHECK_CLOSE(extents(1), 6, eps);
    BOOST_CHECK_CLOSE(extents(2), 8, eps);

    BOOST_CHECK_CLOSE(e.getVolume(), 3*3*4 * M_PI * 4 / 3.0, eps);
}
