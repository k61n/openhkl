#define BOOST_TEST_MODULE "Test Direct Vector"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/mathematics/DirectVector.h>

int run_test() {

    Eigen::Vector3d v1(1,2,3);

    nsx::DirectVector dv1(v1);

    Eigen::Vector3d v2 = v1 + static_cast<const Eigen::Vector3d&>(dv1);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_DirectVector)
{
    BOOST_CHECK(run_test() == 0);
}
