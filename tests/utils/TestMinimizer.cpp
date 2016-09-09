#define BOOST_TEST_MODULE "Test Minimizer class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "Minimizer.h"

#include <Eigen/Dense>

using namespace SX::Utils;
using namespace std;


int run_test()
{
    Minimizer m;
    Eigen::VectorXd x, y, wt;

    int nparams = 3, nvalues = 40;

    const int num_points = 40;
    y.resize(num_points, 1);
    wt.resize(num_points, 1);

    x.resize(3,1);
    x << 4.0, 0.2, 0.5;

    for (int i = 0; i < nvalues; i++)
    {
        double t = i;
        double yi = 1.0 + 5 * exp (-0.1 * t);

        wt[i] = 1.0;
        y[i] = yi;
        cout << "data: " << i << " " << y[i] << " " << endl;
    }

    m.initialize(nparams, nvalues);

    m.setInitialValues(x);
    m.setInitialWeights(wt);

    auto residual_fn = [y] (const Eigen::VectorXd& p, Eigen::VectorXd& r) -> int
    {
        int n = r.rows();

        double A =p(0);
        double lambda = p(1);
        double b =p(2);

        size_t i;

        for (i = 0; i < n; i++)
        {
            /* Model Yi = A * exp(-lambda * i) + b */
            double t = i;
            double Yi = A * exp (-lambda * t) + b;
            r(i) = Yi - y[i];
        }
        return GSL_SUCCESS;
    };

    m.set_f(residual_fn);

    m.fit(200);


    x = m.params();

    BOOST_CHECK_CLOSE(x(0), 5.0, 1e-6);
    BOOST_CHECK_CLOSE(x(1), 0.1, 1e-6);
    BOOST_CHECK_CLOSE(x(2), 1.0, 1e-6);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Minimizer)
{
    BOOST_CHECK(run_test() == 0);
}
