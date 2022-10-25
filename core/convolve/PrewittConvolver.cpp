//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/PrewittConvolver.cpp
//! @brief     Implements class PrewittConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/PrewittConvolver.h"
#include "core/convolve/AtomicConvolver.h"

namespace ohkl {

PrewittConvolver::PrewittConvolver() : AtomicConvolver() { }

PrewittConvolver::PrewittConvolver(const std::map<std::string, double>& parameters)
    : AtomicConvolver(parameters)
{
    _norm_fac = 1.0 / 6.0;
}

Convolver* PrewittConvolver::clone() const
{
    return new PrewittConvolver(*this);
}

std::pair<size_t, size_t> PrewittConvolver::kernelSize() const
{
    return std::make_pair(3, 3);
}

RealMatrix PrewittConvolver::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    if (_parameters.find("x") != _parameters.end()) {
        kernel(0, 0) = 1.0;
        kernel(0, 1) = 1.0;
        kernel(0, 2) = 1.0;
        kernel(2, 0) = -1.0;
        kernel(2, 1) = -1.0;
        kernel(2, 2) = -1.0;
    } else if (_parameters.find("y") != _parameters.end()) {
        kernel(0, 0) = 1.0;
        kernel(1, 0) = 1.0;
        kernel(2, 0) = 1.0;
        kernel(0, 2) = -1.0;
        kernel(1, 2) = -1.0;
        kernel(2, 2) = -1.0;
    } else {
        throw std::runtime_error("Prewitt convolver missing direction parameter (x/y)");
    }

    return kernel * _norm_fac;
}

} // namespace ohkl
