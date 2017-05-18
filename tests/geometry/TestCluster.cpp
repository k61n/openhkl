#define BOOST_TEST_MODULE "Test Cluster"
#define BOOST_TEST_DYN_LINK

#include <Eigen/Dense>

#include <boost/test/unit_test.hpp>

#include <nsxlib/geometry/Cluster.h>

using Eigen::Vector3d;

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_Basis)
{
    Cluster c1(Vector3d(2,0,0),0.02);
    bool test= c1.addVector(Vector3d(2.05,0,0));
    BOOST_CHECK(!test);
    test= c1.addVector(Vector3d(2.03,0,0));
    BOOST_CHECK(test);
}
