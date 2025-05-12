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

MinimizerNLopt::MinimizerNLopt(int nparams, nlopt::vfunc objective, void* f_data)
    : _algo(nlopt::LN_COBYLA)
    , _ftol(1.0e-3)
    , _xtol(1.0e-3)
    , _ctol(1.0e-4)
    , _init_step(0.2)
    , _max_iter(1e7)
{
    _optimizer = nlopt::opt(_algo, nparams);
    _optimizer.set_min_objective(objective, f_data);
}

void MinimizerNLopt::addEqualityConstraint(nlopt::vfunc constraint, void* c_data)
{
    _optimizer.add_equality_constraint(constraint, c_data, _ctol);
}

void MinimizerNLopt::addInequalityConstraint(nlopt::vfunc constraint, void* c_data)
{
    _optimizer.add_inequality_constraint(constraint, c_data, _ctol);
}

std::optional<double> MinimizerNLopt::minimize(std::vector<double>& parameters)
{
    _optimizer.set_ftol_rel(_ftol);
    _optimizer.set_xtol_rel(_xtol);
    _optimizer.set_maxeval(_max_iter);
    _optimizer.set_initial_step(_init_step);
    double minf;
    nlopt::result result;
    try {
        result = _optimizer.optimize(parameters, minf);
    } catch (std::exception& e) {
        ohklLog(Level::Error, "MinimizerNLOpt::minimize: " + std::string(e.what()));
        return {};
    }
    if (result > 0)
        return minf;
    return {};
}


} // namespace ohkl
