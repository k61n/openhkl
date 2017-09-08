#define BOOST_TEST_MODULE "Test Reciprocal Vector"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <nsxlib/mathematics/ReciprocalVector.h>

int run_test() {

    Eigen::RowVector3d v1(1,2,3);

    nsx::ReciprocalVector rv1(v1);

    Eigen::RowVector3d v2 = v1 + static_cast<const Eigen::RowVector3d&>(rv1);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ErfInv)
{
    BOOST_CHECK(run_test() == 0);
}
