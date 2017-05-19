#define BOOST_TEST_MODULE "Test Inverse Error function"
#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <nsxlib/mathematics/ErfInv.h>

using namespace nsx;

int run_test() {

    const double x_max = 5.0;
    const size_t count = 1000;

    for (size_t i = 0; i < count; ++i) {
        const double x = x_max * double(i) / double(count);
        const double y = std::erf(x);

        const double u = erf_inv(y);
        const double v = std::erf(u);

        BOOST_CHECK_CLOSE(u, x, 1e-3);
        BOOST_CHECK_CLOSE(v, y, 1e-13);
    }

    BOOST_CHECK_CLOSE(getScale(0.382925), 0.5, 1e-2);
    BOOST_CHECK_CLOSE(getScale(0.682689), 1.0, 1e-2);
    BOOST_CHECK_CLOSE(getScale(0.866386), 1.5, 1e-2);
    BOOST_CHECK_CLOSE(getScale(0.954500), 2.0, 1e-2);
    BOOST_CHECK_CLOSE(getScale(0.987581), 2.5, 1e-2);
    BOOST_CHECK_CLOSE(getScale(0.997300), 3.0, 1e-2);
    BOOST_CHECK_CLOSE(getScale(0.999535), 3.5, 1e-2);

    BOOST_CHECK_CLOSE(getConfidence(0.5), 0.382925, 1e-2);
    BOOST_CHECK_CLOSE(getConfidence(1.0), 0.682689, 1e-2);
    BOOST_CHECK_CLOSE(getConfidence(1.5), 0.866386, 1e-2);
    BOOST_CHECK_CLOSE(getConfidence(2.0), 0.954500, 1e-2);
    BOOST_CHECK_CLOSE(getConfidence(2.5), 0.987581, 1e-2);
    BOOST_CHECK_CLOSE(getConfidence(3.0), 0.997300, 1e-2);
    BOOST_CHECK_CLOSE(getConfidence(3.5), 0.999535, 1e-2);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_ErfInv)
{
    BOOST_CHECK(run_test() == 0);
}
