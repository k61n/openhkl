//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/SobelConvolver.cpp
//! @brief     Implements class SobelConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/SobelConvolver.h"
#include "core/convolve/AtomicConvolver.h"

namespace ohkl {

SobelConvolver::SobelConvolver() : AtomicConvolver() { }

SobelConvolver::SobelConvolver(const std::map<std::string, double>& parameters)
    : AtomicConvolver(parameters)
{
    _norm_fac = 1.0 / 8.0;
}

Convolver* SobelConvolver::clone() const
{
    return new SobelConvolver(*this);
}

std::pair<size_t, size_t> SobelConvolver::kernelSize() const
{
    return std::make_pair(3, 3);
}

RealMatrix SobelConvolver::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    if (_parameters.find("x") != _parameters.end()) {
        kernel(0, 0) = -1.0;
        kernel(0, 1) = -2.0;
        kernel(0, 2) = -1.0;
        kernel(2, 0) = 1.0;
        kernel(2, 1) = 2.0;
        kernel(2, 2) = 1.0;
    } else if (_parameters.find("y") != _parameters.end()) {
        kernel(0, 0) = 1.0;
        kernel(1, 0) = 2.0;
        kernel(2, 0) = 1.0;
        kernel(0, 2) = -1.0;
        kernel(1, 2) = -2.0;
        kernel(2, 2) = -1.0;
    } else {
        throw std::runtime_error("Sobel convolver missing direction parameter (x/y)");
    }

    return kernel * _norm_fac;
}

} // namespace ohkl
