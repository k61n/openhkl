#define BOOST_TEST_MODULE "Test Gaussian class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <nsxlib/mathematics/Gaussian.h>

using namespace std;
using namespace nsx;

const double eps = 1e-10;

int run_test()
{
    Gaussian g1(2.0, 1.0, 3.0);

    BOOST_CHECK_CLOSE(g1.evaluate(1.0), 4.0, eps);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Gaussian)
{
    BOOST_CHECK(run_test() == 0);
}
