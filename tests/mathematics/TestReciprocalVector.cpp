#define BOOST_TEST_MODULE "Test Direct Vector"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/mathematics/ReciprocalVector.h>

int run_test() {

    nsx::ReciprocalVector rv1(1.0,2.0,3.0);

    BOOST_CHECK_EQUAL(rv1(0),1.0);
    BOOST_CHECK_EQUAL(rv1(1),2.0);
    BOOST_CHECK_EQUAL(rv1(2),3.0);

    nsx::ReciprocalVector rv2(Eigen::Vector3d(5.0,-2.0,4.0));

    rv1 += rv2;

    BOOST_CHECK_EQUAL(rv1(0),6.0);
    BOOST_CHECK_EQUAL(rv1(1),0.0);
    BOOST_CHECK_EQUAL(rv1(2),7.0);

    rv1 *= 10;

    BOOST_CHECK_EQUAL(rv1(0),60.0);
    BOOST_CHECK_EQUAL(rv1(1),0.0);
    BOOST_CHECK_EQUAL(rv1(2),70.0);

    nsx::ReciprocalVector rv3, rv4(1,0,-2), rv5(2,-7,4);

    rv3 = rv4 + rv5;

    BOOST_CHECK_EQUAL(rv3(0), 3.0);
    BOOST_CHECK_EQUAL(rv3(1),-7.0);
    BOOST_CHECK_EQUAL(rv3(2), 2.0);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ErfInv)
{
    BOOST_CHECK(run_test() == 0);
}
