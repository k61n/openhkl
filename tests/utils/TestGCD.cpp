#define BOOST_TEST_MODULE "Test GCD Function"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <nsxlib/utils/gcd.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_GCD)
{
    BOOST_CHECK(gcd(3, 6) == 3);
    BOOST_CHECK(gcd(3, 5) == 1);
    BOOST_CHECK(gcd(10, 25, 15) == 5);
    BOOST_CHECK(gcd(3, -3) == 3);
    BOOST_CHECK(gcd(4, 12, 16) == 4);
    BOOST_CHECK(gcd(12, 4, 16) == 4);
    BOOST_CHECK(gcd(16, 12, 24) == 4);
    BOOST_CHECK(gcd(16, 0) == 16);
    BOOST_CHECK(gcd(0, 16) == 16);
}
