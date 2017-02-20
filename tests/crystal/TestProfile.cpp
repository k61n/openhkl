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

using namespace SX::Crystal;
using namespace std;

const double eps = 5e-1;

int run_test()
{
    std::default_random_engine gen;
    std::uniform_real_distribution<double> dist(-1.0,1.0);

    const int size = 20;
    Eigen::VectorXd x(size);
    Eigen::VectorXd y(size);

    const double a = 50.2345;
    const double b = 10.4321;
    const double x0 = size / 2.0 + 0.987;
    const double x1 = size / 8.0 + 0.123;

    Profile lor(a, b, x0);
    Profile lor_fit;

    //BOOST_CHECK_CLOSE(lor.evaluate(4.5), 0.6355575028687533, eps);
    //BOOST_CHECK_CLOSE(lor.evaluate(8.5), 0.2343922662816658, eps);

    // check that it correctly fits simulated Lorentzian data
    for (auto i = 0; i < size; ++i) {
        y(i) = lor.evaluate(double(i)); // + 0.01*dist(gen);
    }
    BOOST_CHECK(lor_fit.fit(y, 100));
    BOOST_CHECK_CLOSE(lor_fit.getA(), a, eps);
    BOOST_CHECK_CLOSE(lor_fit.getB(), b, eps);
    BOOST_CHECK_CLOSE(lor_fit.getX(), x0, eps);

    // check that it correctly fits simulated Gaussian data
    for (auto i = 0; i < size; ++i) {
        double u = i-x0;
        y(i) = std::exp(-u*u);
    }
    BOOST_CHECK(lor_fit.fit(y, 100));
    BOOST_CHECK_CLOSE(lor_fit.getA(), 0.67638335678034889, eps);
    BOOST_CHECK_CLOSE(lor_fit.getB(), 0.67308471157491034, eps);
    BOOST_CHECK_CLOSE(lor_fit.getX(), 10.972031216972477, eps);

    // check that it correctly fails to fit noise
    for (auto i = 0; i < size; ++i) {
        double u = i-x0;
        y(i) = i%2 == 0 ? -1.0 : 1.0;
    }
    BOOST_CHECK(lor_fit.fit(y, 100) == false);

    // check that it correctly fits a partial profile
    lor.setParams(a, b, x1);
    for (auto i = 0; i < size; ++i) {
        y(i) = lor.evaluate(double(i)); // + 0.01*dist(gen);
    }
    BOOST_CHECK(lor_fit.fit(y, 100));
    BOOST_CHECK_CLOSE(lor_fit.getA(), a, eps);
    BOOST_CHECK_CLOSE(lor_fit.getB(), b, eps);
    BOOST_CHECK_CLOSE(lor_fit.getX(), x1, eps);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Lorentzian)
{
    BOOST_CHECK(run_test() == 0);
}
