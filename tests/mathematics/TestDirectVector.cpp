#define BOOST_TEST_MODULE "Test Direct Vector"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/mathematics/DirectVector.h>

int run_test() {

    nsx::DirectVector dv1(1.0,2.0,3.0);

    BOOST_CHECK_EQUAL(dv1(0),1.0);
    BOOST_CHECK_EQUAL(dv1(1),2.0);
    BOOST_CHECK_EQUAL(dv1(2),3.0);

    nsx::DirectVector dv2(5.0,-2.0,4.0);

    dv1 += dv2;

    BOOST_CHECK_EQUAL(dv1(0),6.0);
    BOOST_CHECK_EQUAL(dv1(1),0.0);
    BOOST_CHECK_EQUAL(dv1(2),7.0);

    dv1 *= 10;

    BOOST_CHECK_EQUAL(dv1(0),60.0);
    BOOST_CHECK_EQUAL(dv1(1),0.0);
    BOOST_CHECK_EQUAL(dv1(2),70.0);

    nsx::DirectVector dv3, dv4(1.0,0.0,-2.0), dv5(2.0,-7.0,4.0);

    dv3 = dv4 + dv5;

    BOOST_CHECK_EQUAL(dv3(0), 3.0);
    BOOST_CHECK_EQUAL(dv3(1),-7.0);
    BOOST_CHECK_EQUAL(dv3(2), 2.0);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ErfInv)
{
    BOOST_CHECK(run_test() == 0);
}
