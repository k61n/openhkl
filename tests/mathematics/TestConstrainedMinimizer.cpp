#include <vector>
#include <string>

#include <Eigen/Dense>

#include <nsxlib/FitParameters.h>
#include <nsxlib/Minimizer.h>
#include <nsxlib/NSXTest.h>

int main()
{
    Eigen::VectorXd y, wt, x;
    const int num_points = 400;
    y.resize(num_points, 1);
    wt.resize(num_points, 1);

    x.resize(3);
    // constraint x(0) == 10*x(1)
    x << 4.01, 0.399, 0.5;

    for (int i = 0; i < num_points; i++) {
        double t = i;
        double yi = x(2) + x(0) * exp (-x(1) * t);

        wt[i] = 1.0;
        y[i] = yi;
    }

    auto residual_fn = [y, &x] (Eigen::VectorXd& r) -> int {
        int n = r.rows();

        double A = x(0);
        double lambda = x(1);
        double b = x(2);

        for (int i = 0; i < n; i++) {
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

    Eigen::MatrixXd constraint(1,3);
    constraint << 1.0, -10.0, 0.0;
    params.setConstraint(constraint);

    nsx::Minimizer min;
    min.initialize(params, 40);
    min.set_f(residual_fn);
    NSX_CHECK_ASSERT(min.fit(100));

    NSX_CHECK_SMALL( (constraint*x).norm(), 1e-6);

    NSX_CHECK_CLOSE(x(0), 4.008, 1e-1);
    NSX_CHECK_CLOSE(x(1), 0.4008, 1e-1);
    NSX_CHECK_CLOSE(x(2), 0.50125, 1e-1);

    std::cout << x.transpose() << std::endl;

    return 0;
}
