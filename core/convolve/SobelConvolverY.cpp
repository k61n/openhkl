//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/SobelConvolverY.cpp
//! @brief     Implements class SobelConvolverY
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/SobelConvolverY.h"

namespace ohkl {

SobelConvolverY::SobelConvolverY() : AtomicConvolver() { }

SobelConvolverY::SobelConvolverY(const std::map<std::string, double>& parameters) : SobelConvolverY()
{
    setParameters(parameters);
}

Convolver* SobelConvolverY::clone() const
{
    return new SobelConvolverY(*this);
}

std::pair<size_t, size_t> SobelConvolverY::kernelSize() const
{
    return std::make_pair(3, 3);
}

RealMatrix SobelConvolverY::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    kernel(0, 0) = 1.0;
    kernel(1, 0) = 2.0;
    kernel(2, 0) = 1.0;
    kernel(0, 2) = -1.0;
    kernel(1, 2) = -2.0;
    kernel(2, 2) = -1.0;

    return kernel;
}

} // namespace ohkl
