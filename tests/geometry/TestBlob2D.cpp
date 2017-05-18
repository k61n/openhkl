#define BOOST_TEST_MODULE "Test 2D Blob"
#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/Blob2D.h>

using namespace nsx;

const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_2D_Blob)
{
    Blob2D blob;
    // Create a set of points for a 2D Gaussian.
    double c_x=12.0;
    double c_y=15.0;
    double sx2=2.0, sy2=3.0;
    double prefactor=0.5/(M_PI*sqrt(sx2*sy2));
    double tot=0;
    for (int i=0;i<50;++i) {
        for (int j=0;j<50;++j) {
            double mass=prefactor*exp(-0.5*(std::pow(i-c_x,2)/sx2+std::pow(j-c_y,2)/sy2));
            blob.addPoint(i,j,mass);
            tot+=mass;
        }
    }
    BOOST_CHECK_CLOSE(tot,1.0,tolerance);
    BOOST_CHECK_CLOSE(tot,blob.getMass(),tolerance);
    Eigen::Vector2d center,eigVal;
    Eigen::Matrix2d eigVec;
    double sigma1=0.682689492;
    blob.toEllipse(sigma1,center,eigVal,eigVec);
    // Check that the center is OK
    BOOST_CHECK_CLOSE(center(0),c_x,tolerance);
    BOOST_CHECK_CLOSE(center(1),c_y,tolerance);
    // Check the semi_axes
    BOOST_CHECK_CLOSE(eigVal(0),sqrt(2.0),tolerance);
    BOOST_CHECK_CLOSE(eigVal(1),sqrt(3.0),tolerance);
    // Check the eigenVectors
    BOOST_CHECK_CLOSE(std::abs((eigVec.col(0))(0)),1.0,tolerance);
    BOOST_CHECK_CLOSE(std::abs((eigVec.col(1))(1)),1.0,tolerance);
}
