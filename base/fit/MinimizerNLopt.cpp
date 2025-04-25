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

#include <iostream>

namespace ohkl {

NLoptFitData::NLoptFitData(unsigned int size)
    : npoints(size)
    , x_data(npoints, 0)
    , y_data(npoints, 0)
{
}

MinimizerNLopt::MinimizerNLopt(unsigned int nparams, unsigned int ndatapoints)
    : _nparams(nparams)
    , _algo(nlopt::LN_AUGLAG)
    , _optimizer(_algo, _nparams)
    , _ftol(1.0e-3)
    , _data(ndatapoints)
{
    setFTol(_ftol);
}

void MinimizerNLopt::reset() { }

void MinimizerNLopt::setObjectiveFunction(nlopt::vfunc func)
{
    _optimizer.set_min_objective(func, &_data);
}

void MinimizerNLopt::setFTol(double ftol)
{
    _optimizer.set_ftol_rel(ftol);
}

bool MinimizerNLopt::minimize(std::vector<double>& parameters)
{
    double minf;
    nlopt::result result;
    bool success = false;
    try {
        result = _optimizer.optimize(parameters, minf);
    } catch (std::exception& e) {
        ohklLog(Level::Error, "MinimizerNLOpt::minimize: " + std::string(e.what()));
        return false;
    }
    if (result > 0)
        success = true;
    return success;
}


} // namespace ohkl
