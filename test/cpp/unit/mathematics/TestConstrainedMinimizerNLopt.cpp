//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/mathematics/TestConstrainedMinimizerNLopt.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/fit/MinimizerNLopt.h"

#include <vector>

namespace {

double modelFunc(std::vector<double> params, double xval)
{
    // y = A * exp(-lambda * i) + b
    return (params[0] * exp(-params[1] * xval)) + params[2];
}

double objective(const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    ohkl::NLoptFitData* fit_data = static_cast<ohkl::NLoptFitData*>(f_data);
    double sum2 = 0;
    for (std::size_t i = 0; i < fit_data->npoints; ++i) {
        double Yi = modelFunc(params, fit_data->x_data[i]);
        sum2 += pow((Yi - fit_data->y_data[i]), 2);
    }
    return sum2;
}

double constraint(const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    (void)f_data;
    // constraint A == 10 * lambda, or x[0] = 10 * x[1]
    return params[0] - (10 * params[1]);
}

}


TEST_CASE("test/mathematics/TestConstrainedMinimizerNLopt.cpp", "")
{
    const int nparams = 3;
    const int npoints = 40;
    ohkl::NLoptFitData data(npoints);


    // Initial guess
    std::vector<double> params = {4.01, 0.399, 0.5};

    // Target parameters
    const std::vector<double> ref_params = {5.0, 0.2, 1.0};

    // Constrained reference parameters
    const std::vector<double> constrained_ref_params =
        {4.3958110707, 0.4395811071, 1.3803889979};

    // Generate data using target parameters
    for (int i = 0; i < npoints; ++i) {
        data.x_data[i] = static_cast<double>(i);
        data.y_data[i] = modelFunc(ref_params, static_cast<double>(i));
    }

    ohkl::MinimizerNLopt minimizer(nparams, objective, &data);
    minimizer.setFTol(1.0e-8);
    minimizer.addEqualityConstraint(constraint, nullptr);
    std::optional<double> minf = minimizer.minimize(params);
    CHECK(minf);

    const double eps = 1.0e-5;
    const double ref_minf = 10.0459822135;

    CHECK_THAT(minf.value(), Catch::Matchers::WithinAbs(ref_minf, eps));
    CHECK_THAT(params[0], Catch::Matchers::WithinAbs(10 * params[1], eps));
    for (std::size_t i = 0; i < nparams; ++i)
        CHECK_THAT(params[i], Catch::Matchers::WithinAbs(constrained_ref_params[i], eps));
}
