#define BOOST_TEST_MODULE "Test Inverse Error function"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <cmath>
#include <nsxlib/utils/erf_inv.h>

using SX::Utils::erf_inv;

int run_test() {

    const double x_max = 5.0;
    const size_t count = 1000;

    for (auto i = 0; i < count; ++i) {
        const double x = x_max * double(i) / double(count);
        const double y = std::erf(x);

        const double u = erf_inv(y);
        const double v = std::erf(u);

        BOOST_CHECK_CLOSE(u, x, 1e-3);
        BOOST_CHECK_CLOSE(v, y, 1e-13);
    }

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ErfInv)
{
    BOOST_CHECK(run_test() == 0);
}
