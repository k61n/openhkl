#include <string>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/SparseCore>

#include "core/fitting/FitParameters.h"
#include "core/fitting/Minimizer.h"

NSX_INIT_TEST

int main()
{
    Eigen::VectorXd y, wt, x;
    const int num_points = 400;
    const int num_dummy = 2000;
    y.resize(num_points, 1);
    wt.resize(num_points, 1);

    x.resize(3 + num_dummy);
    x.setZero();
    // constraint x(0) == 10*x(1)
    x(0) = 4.01;
    x(1) = 0.399;
    x(2) = 0.5;

    for (int i = 0; i < num_points; i++) {
        double t = i;
        double yi = x(2) + x(0) * exp(-x(1) * t);

        wt[i] = 1.0;
        y[i] = yi;
    }

    auto residual_fn = [y, &x](Eigen::VectorXd& r) -> int {
        int n = r.rows();

        double A = x(0);
        double lambda = x(1);
        double b = x(2);

        for (int i = 0; i < n; i++) {
            /* Model Yi = A * exp(-lambda * i) + b */
            double t = i;
            double Yi = A * exp(-lambda * t) + b;
            r(i) = Yi - y[i];
        }
        return 0;
    };

    nsx::FitParameters params;

    for (auto i = 0; i < x.size(); ++i) {
        params.addParameter(&x(i));
    }

    Eigen::SparseMatrix<double> constraint;
    constraint.setZero();
    constraint.resize(2 + num_dummy, 3 + num_dummy);

    constraint.coeffRef(0, 0) = 1.0;
    constraint.coeffRef(0, 1) = -10.0;
    constraint.coeffRef(0, 2) = 0.0;

    constraint.coeffRef(1, 3) = 0.0;

    for (auto i = 1; i < num_dummy; ++i) {
        constraint.coeffRef(i + 2, i + 3) = 1.0;
        constraint.coeffRef(i + 2, i + 2) = 1.0;
    }

    params.setConstraint(constraint);

    nsx::Minimizer min;
    min.initialize(params, 40);
    min.set_f(residual_fn);
    NSX_CHECK_ASSERT(min.fit(100));

    NSX_CHECK_SMALL((constraint * x).norm(), 1e-6);

    NSX_CHECK_CLOSE(x(0), 4.008, 1e-1);
    NSX_CHECK_CLOSE(x(1), 0.4008, 1e-1);
    NSX_CHECK_CLOSE(x(2), 0.50125, 1e-1);

    for (auto i = 3; i < x.size(); ++i) {
        NSX_CHECK_SMALL(x(i), 1e-6);
    }

    return 0;
}
