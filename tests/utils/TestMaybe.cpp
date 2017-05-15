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
    Maybe<int> x(5);

    BOOST_CHECK_EQUAL(x.get(), 5);

    int& y = x.get();
    y = 6;

    BOOST_CHECK_EQUAL(x.get(), 6);

    // class with deleted default constructor
    class TestClass {
    public:
        TestClass() = delete;
        ~TestClass() {}
    };

    // check that we can create 'nothing' of type TestClass
    Maybe<TestClass> test;

    // check that we can create maybe of string type (non-trivial destructor)
    Maybe<std::string> str_nothing;
    Maybe<std::string> str_something("something");

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Gaussian)
{
    BOOST_CHECK(run_test() == 0);
}
