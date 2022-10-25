//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/Sobel5Convolver.cpp
//! @brief     Implements class Sobel5Convolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/Sobel5Convolver.h"
#include "core/convolve/AtomicConvolver.h"

namespace ohkl {

Sobel5Convolver::Sobel5Convolver() : AtomicConvolver() { }

Sobel5Convolver::Sobel5Convolver(const std::map<std::string, double>& parameters)
    : AtomicConvolver(parameters)
{
    _norm_fac = 1.0 / 20.0;
}

Convolver* Sobel5Convolver::clone() const
{
    return new Sobel5Convolver(*this);
}

std::pair<size_t, size_t> Sobel5Convolver::kernelSize() const
{
    return std::make_pair(5, 5);
}

RealMatrix Sobel5Convolver::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    if (_parameters.find("x") != _parameters.end()) {
        kernel(0, 0) = 5;
        kernel(0, 1) = 8;
        kernel(0, 2) = 10;
        kernel(0, 3) = 8;
        kernel(0, 4) = 5;
        kernel(1, 0) = 4;
        kernel(1, 1) = 10;
        kernel(1, 2) = 20;
        kernel(1, 3) = 10;
        kernel(1, 4) = 4;
        kernel(3, 0) = -4;
        kernel(3, 1) = -10;
        kernel(3, 2) = -20;
        kernel(3, 3) = -10;
        kernel(3, 4) = -4;
        kernel(4, 0) = -5;
        kernel(4, 1) = -8;
        kernel(4, 2) = -10;
        kernel(4, 3) = -8;
        kernel(4, 4) = -5;
    } else if (_parameters.find("y") != _parameters.end()) {
        kernel(0, 0) = 5;
        kernel(1, 0) = 8;
        kernel(2, 0) = 10;
        kernel(3, 0) = 8;
        kernel(4, 0) = 5;
        kernel(0, 1) = 4;
        kernel(1, 1) = 10;
        kernel(2, 1) = 20;
        kernel(3, 1) = 10;
        kernel(4, 1) = 4;
        kernel(0, 3) = -4;
        kernel(1, 3) = -10;
        kernel(2, 3) = -20;
        kernel(3, 3) = -10;
        kernel(4, 3) = -4;
        kernel(0, 4) = -5;
        kernel(1, 4) = -8;
        kernel(2, 4) = -10;
        kernel(3, 4) = -8;
        kernel(4, 4) = -5;
    } else {
        throw std::runtime_error("Sobel5 convolver missing direction parameter (x/y)");
    }

    return kernel * _norm_fac;
}

} // namespace ohkl
