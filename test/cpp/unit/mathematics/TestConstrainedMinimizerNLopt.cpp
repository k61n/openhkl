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
    ohkl::MinimizerNLopt minimizer(nparams, npoints);
    ohkl::NLoptFitData* data = minimizer.data();


    // Initial guess
    std::vector<double> params(nparams, 0.0);
    params[0] = 4.01;
    params[1] = 0.399;
    params[2] = 0.5;

    // Target parameters
    std::vector<double> ref_params(nparams, 0.0);
    ref_params[0] = 5.0;
    ref_params[1] = 0.1;
    ref_params[2] = 1.0;

    // Constrained reference parameters
    std::vector<double> constrained_ref_params(nparams, 0.0);
    constrained_ref_params[0] = 3.5892173806;
    constrained_ref_params[1] = 0.3589217376;
    constrained_ref_params[2] = 1.9945771731;

    // Generate data using target parameters
    for (int i = 0; i < npoints; ++i) {
        data->x_data[i] = static_cast<double>(i);
        data->y_data[i] = modelFunc(ref_params, static_cast<double>(i));
    }

    // Give the minimizer initial guess parameters
    minimizer.setObjectiveFunction(objective);
    minimizer.setConstraintFunction(constraint);
    std::optional<double> minf = minimizer.minimize(params);
    CHECK(minf);


    const double eps = 1.0e-6;
    const double ref_minf = 26.1403011495;

    CHECK_THAT(minf.value(), Catch::Matchers::WithinAbs(ref_minf, eps));
    CHECK_THAT(params[0], Catch::Matchers::WithinAbs(10 * params[1], eps));
    for (std::size_t i = 0; i < nparams; ++i)
        CHECK_THAT(params[i], Catch::Matchers::WithinAbs(constrained_ref_params[i], eps));
}
