#define BOOST_TEST_MODULE "Test Direct Vector"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/mathematics/DirectVector.h>

int run_test() {

    nsx::DirectVector v(1,0,0);
    nsx::DirectVector v1(Eigen::Vector3d(1,0,0));

    v += v1;

    std::cout<<v[0]<<std::endl;
    std::cout<<v[1]<<std::endl;
    std::cout<<v[2]<<std::endl;

    v *= 10;

    std::cout<<v[0]<<std::endl;
    std::cout<<v[1]<<std::endl;
    std::cout<<v[2]<<std::endl;

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ErfInv)
{
    BOOST_CHECK(run_test() == 0);
}
