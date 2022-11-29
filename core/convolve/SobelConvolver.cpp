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

namespace ohkl {

SobelConvolver::SobelConvolver() : GradientConvolver() { }

SobelConvolver::SobelConvolver(const std::map<std::string, double>& parameters)
    : GradientConvolver(parameters)
{
    _norm_fac = 1.0 / 8.0;

    Eigen::Matrix3d sobel_x, sobel_y;

    sobel_x << -1, 0, 1, -2, 0, 2, -1, 0, 1;

    sobel_y << 1, 2, 1, 0, 0, 0, -1, -2, -1;

    setOperator(sobel_x, sobel_y);
}

Convolver* SobelConvolver::clone() const
{
    return new SobelConvolver(*this);
}

} // namespace ohkl
