//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/CentralDifference.cpp
//! @brief     Implements class CentralDifference
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/CentralDifference.h"
#include "core/convolve/AtomicConvolver.h"

namespace ohkl {

CentralDifference::CentralDifference() : AtomicConvolver() { }

CentralDifference::CentralDifference(const std::map<std::string, double>& parameters)
    : AtomicConvolver(parameters)
{
}

Convolver* CentralDifference::clone() const
{
    return new CentralDifference(*this);
}

std::pair<size_t, size_t> CentralDifference::kernelSize() const
{
    return std::make_pair(3, 3);
}

RealMatrix CentralDifference::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    if (_parameters.find("x") != _parameters.end()) {
        kernel(0, 1) = -1.0;
        kernel(2, 1) = 1.0;
    } else if (_parameters.find("y") != _parameters.end()) {
        kernel(1, 0) = 1.0;
        kernel(1, 2) = -1.0;
    } else {
        throw std::runtime_error("Sobel convolver missing direction parameter (x/y)");
    }

    return kernel;
}

} // namespace ohkl
