//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/RobertsConvolver.cpp
//! @brief     Implements class RobertsConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/RobertsConvolver.h"
#include "core/convolve/AtomicConvolver.h"

namespace ohkl {

RobertsConvolver::RobertsConvolver() : AtomicConvolver() { }

RobertsConvolver::RobertsConvolver(const std::map<std::string, double>& parameters)
    : AtomicConvolver(parameters)
{
}

Convolver* RobertsConvolver::clone() const
{
    return new RobertsConvolver(*this);
}

std::pair<size_t, size_t> RobertsConvolver::kernelSize() const
{
    return std::make_pair(2, 2);
}

RealMatrix RobertsConvolver::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    if (_parameters.find("x") != _parameters.end()) {
        kernel(0, 0) = 1.0;
        kernel(1, 1) = -1.0;
    } else if (_parameters.find("y") != _parameters.end()) {
        kernel(0, 1) = -1.0;
        kernel(1, 0) = 1.0;
    } else {
        throw std::runtime_error("Roberts convolver missing direction parameter (x/y)");
    }

    return kernel;
}

} // namespace ohkl
