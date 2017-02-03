#define BOOST_TEST_MODULE "Test Cluster"
#define BOOST_TEST_DYN_LINK

#include <nsxlib/geometry/Cluster.h>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>
#include <memory>

using namespace SX::Geometry;
using Eigen::Vector3d;
// const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_Basis)
{
    Cluster c1(Vector3d(2,0,0),0.02);
    bool test= c1.addVector(Vector3d(2.05,0,0));
    BOOST_CHECK(!test);
    test= c1.addVector(Vector3d(2.03,0,0));
    BOOST_CHECK(test);
}
