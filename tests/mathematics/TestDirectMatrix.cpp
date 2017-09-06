#define BOOST_TEST_MODULE "Test Direct Matrix"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <nsxlib/mathematics/DirectMatrix.h>
#include <nsxlib/mathematics/DirectVector.h>

int run_test() {

    nsx::DirectVector dv1(1.0,2.0,3.0);
    nsx::DirectVector dv2(2.0,4.0,7.0);
    nsx::DirectVector dv3(0.0,1.0,6.0);

    nsx::DirectMatrix dmat(dv1,dv2,dv3);

    BOOST_CHECK_EQUAL(dmat(0,0),1.0);
    BOOST_CHECK_EQUAL(dmat(0,1),2.0);
    BOOST_CHECK_EQUAL(dmat(0,2),0.0);
    BOOST_CHECK_EQUAL(dmat(1,0),2.0);
    BOOST_CHECK_EQUAL(dmat(1,1),4.0);
    BOOST_CHECK_EQUAL(dmat(1,2),1.0);
    BOOST_CHECK_EQUAL(dmat(2,0),3.0);
    BOOST_CHECK_EQUAL(dmat(2,1),7.0);
    BOOST_CHECK_EQUAL(dmat(2,2),6.0);

    nsx::DirectVector dv4(-1,3,5);

    nsx::DirectVector dv5 = dmat * dv4;
    BOOST_CHECK_EQUAL(dv5(0),5);
    BOOST_CHECK_EQUAL(dv5(1),15);
    BOOST_CHECK_EQUAL(dv5(2),48);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ErfInv)
{
    BOOST_CHECK(run_test() == 0);
}
