//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/mathematics/TestMinimizer.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "base/fit/FitParameters.h"
#include "base/fit/Minimizer.h"

TEST_CASE("test/mathematics/TestMinimizer.cpp", "")
{
    Eigen::VectorXd y, wt, x;
    int nvalues = 40;
    const int num_points = 40;
    y.resize(num_points, 1);
    wt.resize(num_points, 1);

    x.resize(3);
    x << 4.0, 0.2, 0.5;

    for (int i = 0; i < nvalues; i++) {
        double t = i;
        double yi = 1.0 + 5 * exp(-0.1 * t);

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

    ohkl::FitParameters params;
    params.addParameter(&x(0));
    params.addParameter(&x(1));
    params.addParameter(&x(2));

    ohkl::Minimizer min;
    min.initialize(params, 40);
    min.set_f(residual_fn);
    CHECK(min.fit(100));

    CHECK(x(0) == Approx(5.0).epsilon(1e-6));
    CHECK(x(1) == Approx(0.1).epsilon(1e-6));
    CHECK(x(2) == Approx(1.0).epsilon(1e-6));
}
