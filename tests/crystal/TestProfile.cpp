#define BOOST_TEST_MODULE "Test Profile class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>

#include <nsxlib/crystal/Profile.h>
#include <Eigen/Dense>

//using namespace SX::Crystal;
//using namespace std;

using SX::Crystal::Profile;
using SX::Utils::Lorentzian;
using SX::Utils::Gaussian;

const double eps = 1.0;

#define fit_and_test(y) \
{ \
    Profile p; \
    BOOST_CHECK(p.fit(y, 100) == true); \
    for (auto i = 0; i < y.size(); ++i) { \
        BOOST_CHECK_CLOSE(y(i), p.evaluate(double(i)), eps); \
    } \
}

int run_test()
{
    const int size = 50;
    Eigen::VectorXd lorentz_test(size);
    Eigen::VectorXd gauss_test(size);
    Eigen::VectorXd voigt_test(size);
    Eigen::VectorXd noise_test(size);

    Lorentzian lor(4.0, 1.0, 14.0);
    Gaussian gauss(2.0, 14.0, 3.0);

    // simulated data
    for (auto i = 0; i < size; ++i) {
        const double x(i);
        lorentz_test(i) = 0.9*lor.evaluate(x)+0.1*gauss.evaluate(x);
        gauss_test(i) = 0.1*lor.evaluate(x)+0.9*gauss.evaluate(x);
        voigt_test(i) = 0.5*lorentz_test(i) + 0.5*gauss_test(i);
        noise_test(i) = (i%2) == 0 ? -1 : 1;
    }

    fit_and_test(lorentz_test);
    fit_and_test(gauss_test);
    fit_and_test(voigt_test);
    BOOST_CHECK(Profile().fit(noise_test, 100) == false);
    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Profile)
{
    BOOST_CHECK(run_test() == 0);
}
