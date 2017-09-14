#define BOOST_TEST_MODULE "Test Miller Indices"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/MillerIndices.h>

int run_test() {

    Eigen::RowVector3i v1(1,2,3);

    nsx::MillerIndices hkl1(v1);

    Eigen::RowVector3i v2 = v1 + static_cast<const Eigen::RowVector3i&>(hkl1);

    int& x = hkl1[0];

    x = 100;

    BOOST_CHECK_EQUAL(hkl1[0],100);

    Eigen::RowVector3d hkld = static_cast<Eigen::RowVector3d>(hkl1);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_MillerIndices)
{
    BOOST_CHECK(run_test() == 0);
}
