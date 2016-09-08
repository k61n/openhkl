#define BOOST_TEST_MODULE "Test Minimizer class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <random>

#include "Minimizer.h"

#include <Eigen/Dense>

using namespace SX::Utils;
using namespace std;

class exp_functor
{
public:
    exp_functor()
    {
        const int num_points = 40;
        _y.resize(num_points, 1);
        _wt.resize(num_points, 1);
        gen = std::mt19937(rd());

        _x.resize(3,1);
        _x << 4.0, 0.2, 0.5;

        for (int i = 0; i < num_points; i++)
        {
            double t = i;
            double yi = 1.0 + 5 * exp (-0.1 * t);
            double si = 0.1 * yi;

            r = std::normal_distribution<>(0, si);
            double dy = r(gen);

            _wt[i] = 1.0 / (si * si);
            _y[i] = yi + dy;
            cout << "data: " << i << " " << _y[i] << " " << si << endl;
        }
    }

    exp_functor(const exp_functor& other):
        _x(other._x), _y(other._y), _wt(other._wt),
        rd(), gen(other.gen), r(other.r)
    {}

    int operator()(const Eigen::VectorXd& x, Eigen::VectorXd& f)
    {
        int n = f.rows();

        double A =x(0);
        double lambda = x(1);
        double b =x(2);

        size_t i;

        for (i = 0; i < n; i++)
        {
            /* Model Yi = A * exp(-lambda * i) + b */
            double t = i;
            double Yi = A * exp (-lambda * t) + b;
            f(i) = Yi - _y[i];
        }
        return GSL_SUCCESS;
    }


    Eigen::VectorXd _x, _y, _wt;
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<> r;
};

int run_test()
{
    exp_functor f;
    Minimizer m;

    m.init(f._x.size(), f._y.size());

    m.setInitialValues(f._x);
    m.setInitialWeights(f._wt);
    m.set_f(f);

    m.fit(200);
    m.free();

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Minimizer)
{
    BOOST_CHECK(run_test() == 0);
}
