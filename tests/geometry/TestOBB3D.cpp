#define BOOST_TEST_MODULE "Test 3D Object oriented Bounding Box"
#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/OBB.h>

const double tolerance=1e-5;
const double tolerance_large=1.0;

BOOST_AUTO_TEST_CASE(Test_OBB_3D)
{
    // Test: the construction onf an OBB
    Eigen::Vector3d center(3,4,7);
    Eigen::Vector3d extent(sqrt(2),sqrt(2),1);
    Eigen::Matrix3d axis;
    axis << 1,0,0,
            0,1,0,
            0,0,1;
    nsx::OBB obb1(center,extent,axis);
    Eigen::Vector3d lower(obb1.getLower());
    Eigen::Vector3d upper(obb1.getUpper());

    BOOST_CHECK_CLOSE(lower[0],3.0-sqrt(2),tolerance);
    BOOST_CHECK_CLOSE(lower[1],4.0-sqrt(2),tolerance);
    BOOST_CHECK_CLOSE(lower[2],6.0,tolerance);
    BOOST_CHECK_CLOSE(upper[0],3.0+sqrt(2),tolerance);
    BOOST_CHECK_CLOSE(upper[1],4.0+sqrt(2),tolerance);
    BOOST_CHECK_CLOSE(upper[2],8.0,tolerance);

    // Test: the rotation of an OBB
    Eigen::Matrix3d r;
    r << 1,-1,0,
         1, 1,0,
         0, 0,1;
    obb1.rotate(r);
    lower=obb1.getLower();
    upper=obb1.getUpper();

    BOOST_CHECK_CLOSE(lower[0],1.0,tolerance);
    BOOST_CHECK_CLOSE(lower[1],2.0,tolerance);
    BOOST_CHECK_CLOSE(lower[2],6.0,tolerance);
    BOOST_CHECK_CLOSE(upper[0],5.0,tolerance);
    BOOST_CHECK_CLOSE(upper[1],6.0,tolerance);
    BOOST_CHECK_CLOSE(upper[2],8.0,tolerance);

    // Test: the isotropic scaling of an OBB
    center << 3,2,4;
    extent << 1,4,2;
    axis << 1, 0 ,0,
            0, 1 ,0,
            0, 0 ,1;

    nsx::OBB obb2(center,extent,axis);
    obb2.scale(5);
    lower = obb2.getLower();
    upper = obb2.getUpper();
    BOOST_CHECK_CLOSE(lower[0], -2.0,tolerance);
    BOOST_CHECK_CLOSE(lower[1],-18.0,tolerance);
    BOOST_CHECK_CLOSE(lower[2], -6.0,tolerance);
    BOOST_CHECK_CLOSE(upper[0], 8.0,tolerance);
    BOOST_CHECK_CLOSE(upper[1],22.0,tolerance);
    BOOST_CHECK_CLOSE(upper[2],14.0,tolerance);

    // Test: the anisotropic scaling of an OBB
    center << 2,2,1;
    extent << sqrt(2)/2,sqrt(2),2;
    axis << 1,-1 ,0,
            1, 1 ,0,
            0, 0 ,1;

    nsx::OBB obb3(center,extent,axis);
    obb3.scale(Eigen::Vector3d(3,2,5));
    lower=obb3.getLower();
    upper=obb3.getUpper();
    BOOST_CHECK_CLOSE(lower[0],-1.5,tolerance);
    BOOST_CHECK_CLOSE(lower[1],-1.5,tolerance);
    BOOST_CHECK_CLOSE(lower[2],-9.0,tolerance);
    BOOST_CHECK_CLOSE(upper[0], 5.5,tolerance);
    BOOST_CHECK_CLOSE(upper[1], 5.5,tolerance);
    BOOST_CHECK_CLOSE(upper[2],11.0,tolerance);

    // Test: the translation of an OBB
    obb3.translate(Eigen::Vector3d(-1,2,4));
    lower = obb3.getLower();
    upper = obb3.getUpper();
    BOOST_CHECK_CLOSE(lower[0],-2.5,tolerance);
    BOOST_CHECK_CLOSE(lower[1], 0.5,tolerance);
    BOOST_CHECK_CLOSE(lower[2],-5.0,tolerance);
    BOOST_CHECK_CLOSE(upper[0], 4.5,tolerance);
    BOOST_CHECK_CLOSE(upper[1], 7.5,tolerance);
    BOOST_CHECK_CLOSE(upper[2],15.0,tolerance);

    // Test: a given point falls inside the OBB

    obb3.translate(Eigen::Vector3d(1,-2,-4));
    lower=obb3.getLower();
    upper=obb3.getUpper();

    BOOST_CHECK_CLOSE(lower[0],-1.5,tolerance);
    BOOST_CHECK_CLOSE(lower[1],-1.5,tolerance);
    BOOST_CHECK_CLOSE(lower[2],-9.0,tolerance);
    BOOST_CHECK_CLOSE(upper[0], 5.5,tolerance);
    BOOST_CHECK_CLOSE(upper[1], 5.5,tolerance);
    BOOST_CHECK_CLOSE(upper[2],11.0,tolerance);

    const int nSteps(500);
    Eigen::Vector3d delta=(upper-lower)/nSteps;
    Eigen::Vector4d point(0,0,0,1);
    double sum(0.0);
    for(int i = 0; i <= nSteps; ++i) {
        point.x() = lower[0]+i*delta[0];
        for(int j=0;j<=nSteps;++j) {
            point.y() = lower[1]+j*delta[1];
            for(int k=0;k<=nSteps;++k) {
                point.z() = lower[2]+k*delta[2];
                if (obb3.isInside(point))
                    sum+=1.0;
            }
        }
    }

    sum *= ((upper[0]-lower[0])*(upper[1]-lower[1])*(upper[2]-lower[2]))/(nSteps*nSteps*nSteps);

    BOOST_CHECK_CLOSE(sum, 480, tolerance_large);
}
