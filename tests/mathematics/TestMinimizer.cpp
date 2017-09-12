#define BOOST_TEST_MODULE "Test Minimizer class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <vector>
#include <string>

#include <Eigen/Dense>

#include <nsxlib/mathematics/Minimizer.h>
#include <nsxlib/utils/FitParameters.h>

int run_test()
{
    Eigen::VectorXd y, wt, x;
    int nparams = 3, nvalues = 40;
    const int num_points = 40;
    y.resize(num_points, 1);
    wt.resize(num_points, 1);

    x.resize(3);
    x << 4.0, 0.2, 0.5;

    for (int i = 0; i < nvalues; i++) {
        double t = i;
        double yi = 1.0 + 5 * exp (-0.1 * t);

        wt[i] = 1.0;
        y[i] = yi;
    }

    auto residual_fn = [y, &x] (Eigen::VectorXd& r) -> int {
        int n = r.rows();

        double A = x(0);
        double lambda = x(1);
        double b = x(2);

        size_t i;

        for (i = 0; i < n; i++) {
            /* Model Yi = A * exp(-lambda * i) + b */
            double t = i;
            double Yi = A * exp (-lambda * t) + b;
            r(i) = Yi - y[i];
        }
        return 0;
    };
 
    nsx::FitParameters params;
    params.addParameter(&x(0));
    params.addParameter(&x(1));
    params.addParameter(&x(2));

    nsx::Minimizer min;
    min.initialize(params, 40);
    min.set_f(residual_fn);
    BOOST_CHECK(min.fit(100));

    BOOST_CHECK_CLOSE(x(0), 5.0, 1e-6);
    BOOST_CHECK_CLOSE(x(1), 0.1, 1e-6);
    BOOST_CHECK_CLOSE(x(2), 1.0, 1e-6);

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Minimizer)
{
    BOOST_CHECK(run_test() == 0);
}
