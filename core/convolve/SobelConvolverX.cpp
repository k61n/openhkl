//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/SobelConvolverX.cpp
//! @brief     Implements class SobelConvolverX
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/SobelConvolverX.h"

namespace ohkl {

SobelConvolverX::SobelConvolverX() : AtomicConvolver() { }

SobelConvolverX::SobelConvolverX(const std::map<std::string, double>& parameters) : SobelConvolverX()
{
    setParameters(parameters);
}

Convolver* SobelConvolverX::clone() const
{
    return new SobelConvolverX(*this);
}

std::pair<size_t, size_t> SobelConvolverX::kernelSize() const
{
    return std::make_pair(3, 3);
}

RealMatrix SobelConvolverX::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    kernel(0, 0) = -1.0;
    kernel(0, 1) = -2.0;
    kernel(0, 2) = -1.0;
    kernel(2, 0) = 1.0;
    kernel(2, 1) = 2.0;
    kernel(2, 2) = 1.0;

    return kernel;
}

} // namespace ohkl
