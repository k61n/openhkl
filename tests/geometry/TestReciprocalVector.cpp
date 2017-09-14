#define BOOST_TEST_MODULE "Test Reciprocal Vector"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/ReciprocalVector.h>

int run_test() {

    Eigen::RowVector3d v1(1,2,3);

    nsx::ReciprocalVector rv1(v1);

    Eigen::RowVector3d v2 = v1 + static_cast<const Eigen::RowVector3d&>(rv1);

    double& x = rv1[0];

    x = 100;

    BOOST_CHECK_EQUAL(rv1[0],100);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ReciprocalVector)
{
    BOOST_CHECK(run_test() == 0);
}
