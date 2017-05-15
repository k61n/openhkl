#define BOOST_TEST_MODULE "Test Minimizer class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <Eigen/Dense>

#include <nsxlib/utils/MinimizerGSL.h>

using namespace nsx::Utils;
using namespace std;

int run_test()
{
    std::unique_ptr<IMinimizer> m_gsl;

    Eigen::VectorXd x_initial, x_gsl, y, wt;

    int nparams = 3, nvalues = 40;

    const int num_points = 40;
    y.resize(num_points, 1);
    wt.resize(num_points, 1);

    x_gsl.resize(3);
    x_initial.resize(3);

    x_initial << 4.0, 0.2, 0.5;

    for (int i = 0; i < nvalues; i++) {
        double t = i;
        double yi = 1.0 + 5 * exp (-0.1 * t);

        wt[i] = 1.0;
        y[i] = yi;
    }

    auto residual_fn = [y] (const Eigen::VectorXd& p, Eigen::VectorXd& r) -> int {
        int n = r.rows();

        double A = p(0);
        double lambda = p(1);
        double b = p(2);

        size_t i;

        for (i = 0; i < n; i++) {
            /* Model Yi = A * exp(-lambda * i) + b */
            double t = i;
            double Yi = A * exp (-lambda * t) + b;
            r(i) = Yi - y[i];
        }
        return 0;
    };

    m_gsl = std::unique_ptr<IMinimizer>(new MinimizerGSL);

    m_gsl->initialize(nparams, nvalues);
    m_gsl->setParams(x_initial);
    m_gsl->setWeights(wt);
    m_gsl->set_f(residual_fn);
    m_gsl->fit(200);
    x_gsl = m_gsl->params();

    BOOST_CHECK_CLOSE(x_gsl(0), 5.0, 1e-6);
    BOOST_CHECK_CLOSE(x_gsl(1), 0.1, 1e-6);
    BOOST_CHECK_CLOSE(x_gsl(2), 1.0, 1e-6);

    Eigen::MatrixXd j_gsl;

    cout << "Number of iterations:" << endl;
    cout << "    GSL:   " << m_gsl->numIterations() << endl;

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Minimizer)
{
    BOOST_CHECK(run_test() == 0);
}
