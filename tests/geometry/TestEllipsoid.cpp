#include <Eigen/Dense>

#include <nsxlib/AABB.h>
#include <nsxlib/Ellipsoid.h>
#include <nsxlib/NSXTest.h>

const double eps = 1e-8;

int main()
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

    NSX_CHECK_CLOSE(p(0), center(0), eps);
    NSX_CHECK_CLOSE(p(1), center(1), eps);
    NSX_CHECK_CLOSE(p(2), center(2), eps);

    auto lower = e.aabb().lower();

    NSX_CHECK_CLOSE(lower(0), -3+center(0), eps);
    NSX_CHECK_CLOSE(lower(1), -3+center(1), eps);
    NSX_CHECK_CLOSE(lower(2), -4+center(2), eps);

    auto upper = e.aabb().upper();

    NSX_CHECK_CLOSE(upper(0), 3+center(0), eps);
    NSX_CHECK_CLOSE(upper(1), 3+center(1), eps);
    NSX_CHECK_CLOSE(upper(2), 4+center(2), eps);

    auto extents = e.aabb().extents();

    NSX_CHECK_CLOSE(extents(0), 6, eps);
    NSX_CHECK_CLOSE(extents(1), 6, eps);
    NSX_CHECK_CLOSE(extents(2), 8, eps);

    NSX_CHECK_CLOSE(e.volume(), 3*3*4 * 4 * M_PI / 3.0, eps);

    Eigen::Vector3d shift(1,-1,2);

    e.translate(shift);

    p = e.aabb().center();

    NSX_CHECK_CLOSE(p(0), center(0)+shift(0), eps);
    NSX_CHECK_CLOSE(p(1), center(1)+shift(1), eps);
    NSX_CHECK_CLOSE(p(2), center(2)+shift(2), eps);

    lower = e.aabb().lower();

    NSX_CHECK_CLOSE(lower(0), -3+center(0)+shift(0), eps);
    NSX_CHECK_CLOSE(lower(1), -3+center(1)+shift(1), eps);
    NSX_CHECK_CLOSE(lower(2), -4+center(2)+shift(2), eps);

    upper = e.aabb().upper();

    NSX_CHECK_CLOSE(upper(0), 3+center(0)+shift(0), eps);
    NSX_CHECK_CLOSE(upper(1), 3+center(1)+shift(1), eps);
    NSX_CHECK_CLOSE(upper(2), 4+center(2)+shift(2), eps);

    extents = e.aabb().extents();

    NSX_CHECK_CLOSE(extents(0), 6, eps);
    NSX_CHECK_CLOSE(extents(1), 6, eps);
    NSX_CHECK_CLOSE(extents(2), 8, eps);

    NSX_CHECK_CLOSE(e.volume(), 3*3*4 * M_PI * 4 / 3.0, eps);

    const int N = 8;
    const double dx = 4.5 / N;

    int count = 0;

    for (auto i = -N; i < N; ++i) {
        for (auto j = -N; j < N; ++j) {
            for (auto k = -N; k < N; ++k) {
                Eigen::Vector3d u(center(0)+i*dx, center(1)+j*dx, center(2)+k*dx);

                NSX_CHECK_EQUAL(f.isInside(u), (u-center).transpose() * f.metric() * (u-center) < 1.0);

                if (f.isInside(u)) {
                    ++count;
                }                         
            }
        }
    }

    NSX_CHECK_CLOSE(count*dx*dx*dx, f.volume(), 0.1);

    e = f;
    shift << 6.1, 0, 0;
    f.translate(shift);
    NSX_CHECK_EQUAL(e.collide(f), false);
    NSX_CHECK_EQUAL(f.collide(e), false);
    f.translate(-shift/2);
    NSX_CHECK_EQUAL(e.collide(f), true);
    NSX_CHECK_EQUAL(f.collide(e), true);
    f.translate(-shift/2);

    auto fCenter = f.aabb().center();

    NSX_CHECK_CLOSE(fCenter[0], center(0), eps);
    NSX_CHECK_CLOSE(fCenter[1], center(1), eps);
    NSX_CHECK_CLOSE(fCenter[2], center(2), eps);

    e.translate(-shift);

    // AABB center at -18,-18,-18
    nsx::AABB aabb(Eigen::Vector3d(-20,-20,-20),Eigen::Vector3d(-16,-16,-16));
    NSX_CHECK_ASSERT(!f.collide(aabb));

    // AABB center at -16,-16,-16
    aabb.translate(Eigen::Vector3d(2,2,2));
    NSX_CHECK_ASSERT(!f.collide(aabb));

    // AABB center at 0,0,0
    aabb.translate(Eigen::Vector3d(16,16,16));
    NSX_CHECK_ASSERT(!f.collide(aabb));

    // AABB center at 10,10,10
    aabb.translate(Eigen::Vector3d(10,10,10));
    NSX_CHECK_ASSERT(f.collide(aabb));

    // AABB center at 12,10,10
    aabb.translate(Eigen::Vector3d(2,0,0));
    NSX_CHECK_ASSERT(f.collide(aabb));

    // AABB center at 14,10,10
    aabb.translate(Eigen::Vector3d(2,0,0));
    NSX_CHECK_ASSERT(f.collide(aabb));

    // AABB center at 16,10,10
    aabb.translate(Eigen::Vector3d(2,0,0));
    NSX_CHECK_ASSERT(!f.collide(aabb));

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

    const double eps = 1e-10;

    NSX_CHECK_SMALL((Q*R - I).norm(), eps);
    NSX_CHECK_SMALL((R*Q - I).norm(), eps);
    NSX_CHECK_SMALL((Q*QI - I).norm(), eps);
    NSX_CHECK_SMALL((QI*Q - I).norm(), eps);
    NSX_CHECK_SMALL((QI - R).norm(), eps);

    std::cout << "--------------" << std::endl;
    std::cout << Q.inverse() << std::endl;
    std::cout << "--------------" << std::endl;
    std::cout << QI << std::endl;

    
    nsx::AABB box(Eigen::Vector3d(0,0,0), Eigen::Vector3d(1,1,1));

    const double t = 1.0 / std::sqrt(2);
    Eigen::Matrix3d U;

    U << 
    -t, t, 0,
     t, t, 0, 
     0, 0, 1;

    nsx::Ellipsoid el(Eigen::Vector3d(3, -3, 0), Eigen::Vector3d(1.0, 20.0, 100.0), U);

    // check collision with line segments
    NSX_CHECK_EQUAL(el.collideSegment(Eigen::Vector3d(3, -3, 0), Eigen::Vector3d(3, -100, 0)), true);
    NSX_CHECK_EQUAL(el.collideSegment(Eigen::Vector3d(5, -5, 0), Eigen::Vector3d(1, -1, 0)), true);

    // this test fails with the old incorrect collision
    NSX_CHECK_EQUAL(el.aabb().collide(box), true);
    NSX_CHECK_EQUAL(box.collide(el), false);
    NSX_CHECK_EQUAL(el.collide(box), false);
}
