#define BOOST_TEST_MODULE "Test Ellipsoid 3D"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/AABB.h>
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
    nsx::Ellipsoid f(center,semi_axes,eigV);
    
    auto p = e.aabb().center();

    BOOST_CHECK_CLOSE(p(0), center(0), eps);
    BOOST_CHECK_CLOSE(p(1), center(1), eps);
    BOOST_CHECK_CLOSE(p(2), center(2), eps);

    auto lower = e.aabb().lower();

    BOOST_CHECK_CLOSE(lower(0), -3+center(0), eps);
    BOOST_CHECK_CLOSE(lower(1), -3+center(1), eps);
    BOOST_CHECK_CLOSE(lower(2), -4+center(2), eps);

    auto upper = e.aabb().upper();

    BOOST_CHECK_CLOSE(upper(0), 3+center(0), eps);
    BOOST_CHECK_CLOSE(upper(1), 3+center(1), eps);
    BOOST_CHECK_CLOSE(upper(2), 4+center(2), eps);

    auto extents = e.aabb().extents();

    BOOST_CHECK_CLOSE(extents(0), 6, eps);
    BOOST_CHECK_CLOSE(extents(1), 6, eps);
    BOOST_CHECK_CLOSE(extents(2), 8, eps);

    BOOST_CHECK_CLOSE(e.volume(), 3*3*4 * 4 * M_PI / 3.0, eps);

    Eigen::Vector3d shift(1,-1,2);

    e.translate(shift);

    p = e.aabb().center();

    BOOST_CHECK_CLOSE(p(0), center(0)+shift(0), eps);
    BOOST_CHECK_CLOSE(p(1), center(1)+shift(1), eps);
    BOOST_CHECK_CLOSE(p(2), center(2)+shift(2), eps);

    lower = e.aabb().lower();

    BOOST_CHECK_CLOSE(lower(0), -3+center(0)+shift(0), eps);
    BOOST_CHECK_CLOSE(lower(1), -3+center(1)+shift(1), eps);
    BOOST_CHECK_CLOSE(lower(2), -4+center(2)+shift(2), eps);

    upper = e.aabb().upper();

    BOOST_CHECK_CLOSE(upper(0), 3+center(0)+shift(0), eps);
    BOOST_CHECK_CLOSE(upper(1), 3+center(1)+shift(1), eps);
    BOOST_CHECK_CLOSE(upper(2), 4+center(2)+shift(2), eps);

    extents = e.aabb().extents();

    BOOST_CHECK_CLOSE(extents(0), 6, eps);
    BOOST_CHECK_CLOSE(extents(1), 6, eps);
    BOOST_CHECK_CLOSE(extents(2), 8, eps);

    BOOST_CHECK_CLOSE(e.volume(), 3*3*4 * M_PI * 4 / 3.0, eps);

    const int N = 8;
    const double dx = 4.5 / N;

    int count = 0;

    for (auto i = -N; i < N; ++i) {
        for (auto j = -N; j < N; ++j) {
            for (auto k = -N; k < N; ++k) {
                Eigen::Vector3d u(center(0)+i*dx, center(1)+j*dx, center(2)+k*dx);

                BOOST_CHECK_EQUAL(f.isInside(u), (u-center).transpose() * f.metric() * (u-center) < 1.0);

                if (f.isInside(u)) {
                    ++count;
                }                         
            }
        }
    }

    BOOST_CHECK_CLOSE(count*dx*dx*dx, f.volume(), 0.1);

    e = f;
    shift << 6.1, 0, 0;
    f.translate(shift);
    BOOST_CHECK_EQUAL(e.collide(f), false);
    BOOST_CHECK_EQUAL(f.collide(e), false);
    f.translate(-shift/2);
    BOOST_CHECK_EQUAL(e.collide(f), true);
    BOOST_CHECK_EQUAL(f.collide(e), true);
    f.translate(-shift/2);

    auto fCenter = f.aabb().center();

    BOOST_CHECK_CLOSE(fCenter[0], center(0), eps);
    BOOST_CHECK_CLOSE(fCenter[1], center(1), eps);
    BOOST_CHECK_CLOSE(fCenter[2], center(2), eps);

    e.translate(-shift);

    // AABB center at -18,-18,-18
    nsx::AABB aabb(Eigen::Vector3d(-20,-20,-20),Eigen::Vector3d(-16,-16,-16));
    std::cout << "ellipsoid aabb: " << f.aabb().lower().transpose() << " " << f.aabb().upper().transpose() << std::endl;
    BOOST_CHECK(!f.collide(aabb));

    // AABB center at -16,-16,-16
    aabb.translate(Eigen::Vector3d(2,2,2));
    BOOST_CHECK(!f.collide(aabb));

    // AABB center at 0,0,0
    aabb.translate(Eigen::Vector3d(16,16,16));
    BOOST_CHECK(!f.collide(aabb));

    // AABB center at 10,10,10
    aabb.translate(Eigen::Vector3d(10,10,10));
    BOOST_CHECK(f.collide(aabb));

    // AABB center at 12,10,10
    aabb.translate(Eigen::Vector3d(2,0,0));
    BOOST_CHECK(f.collide(aabb));

    // AABB center at 14,10,10
    aabb.translate(Eigen::Vector3d(2,0,0));
    BOOST_CHECK(f.collide(aabb));

    // AABB center at 16,10,10
    aabb.translate(Eigen::Vector3d(2,0,0));
    BOOST_CHECK(!f.collide(aabb));

    // test inverse homogeneous matrix
    Eigen::Vector3d c(15.4, 12.2, 7.5);
    Eigen::Matrix3d m;

    m <<
    16.0, 4.0, -1.0, 
    4.0, 20.0, -3.0,
    -1.0, -3.0, 10.0;

    nsx::Ellipsoid g(c, m);

    Eigen::Matrix4d Q = g.homogeneousMatrix();
    Eigen::Matrix4d QI = g.homogeneousMatrixInverse();
    Eigen::Matrix4d I = Eigen::Matrix4d::Identity();

    Eigen::Matrix4d R = Q.inverse();
    Eigen::Matrix4d S = QI.inverse();

    const double eps = 1e-10;

    BOOST_CHECK_SMALL((Q*R - I).norm(), eps);
    BOOST_CHECK_SMALL((R*Q - I).norm(), eps);
    BOOST_CHECK_SMALL((Q*QI - I).norm(), eps);
    BOOST_CHECK_SMALL((QI*Q - I).norm(), eps);
    BOOST_CHECK_SMALL((QI - R).norm(), eps);

    std::cout << "--------------" << std::endl;
    std::cout << Q.inverse() << std::endl;
    std::cout << "--------------" << std::endl;
    std::cout << QI << std::endl;
}
