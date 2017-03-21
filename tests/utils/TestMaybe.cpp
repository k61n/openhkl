#define BOOST_TEST_MODULE "Test Maybe monad"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>

#include <nsxlib/utils/Maybe.h>

using namespace SX::Utils;
using namespace std;

const double eps = 1e-10;

int run_test()
{
#pragma message "test not implemented"

    Maybe<int> x(5);

    BOOST_CHECK_EQUAL(x.get(), 5);

    int& y = x.get();
    y = 6;

    BOOST_CHECK_EQUAL(x.get(), 6);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Gaussian)
{
    BOOST_CHECK(run_test() == 0);
}
