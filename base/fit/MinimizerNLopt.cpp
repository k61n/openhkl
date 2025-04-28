//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/fit/MinimizerNLopt.cpp
//! @brief     Implements class MinimizerNLopt
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/fit/MinimizerNLopt.h"

#include "base/utils/Logger.h"

#include <nlopt.hpp>

namespace ohkl {

NLoptFitData::NLoptFitData(unsigned int size)
    : npoints(size), x_data(npoints, 0), y_data(npoints, 0)
{
}

MinimizerNLopt::MinimizerNLopt()
    : _algo(nlopt::LN_AUGLAG)
    , _ftol(1.0e-3)
    , _ctol(1.0e-8)
{
}

void MinimizerNLopt::addParameter(double* address)
{
    _parameters.emplace_back(address);
}


std::vector<double> MinimizerNLopt::grabParameters()
{
    std::vector<double> params(_parameters.size(), 0);
    for (std::size_t i = 0; i < params.size(); ++i)
        params[i] = *_parameters[i];
    return params;
}

void MinimizerNLopt::setParameters(const std::vector<double> params)
{
    for (std::size_t i = 0; i < params.size(); ++i)
        *_parameters[i] = params[i];
}

void MinimizerNLopt::init(
    void* f_data, nlopt::vfunc objective, std::optional<nlopt::vfunc> constraint)
{
    _optimizer = nlopt::opt(_algo, _parameters.size());
    _optimizer.set_min_objective(objective, f_data);
    if (constraint)
        _optimizer.add_equality_constraint(constraint.value(), f_data, _ctol);
}

std::optional<double> MinimizerNLopt::minimize()
{
    _optimizer.set_ftol_rel(_ftol);
    double minf;
    nlopt::result result;
    try {
        std::vector<double> parameters = grabParameters();
        result = _optimizer.optimize(parameters, minf);
        setParameters(parameters);
    } catch (std::exception& e) {
        ohklLog(Level::Error, "MinimizerNLOpt::minimize: " + std::string(e.what()));
        return {};
    }
    if (result > 0)
        return minf;
    return {};
}


} // namespace ohkl
