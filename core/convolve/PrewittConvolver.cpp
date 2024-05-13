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

namespace ohkl {

PrewittConvolver::PrewittConvolver() : GradientConvolver() { }

PrewittConvolver::PrewittConvolver(const std::map<std::string, double>& parameters)
    : GradientConvolver(parameters)
{
    _norm_fac = 1.0 / 6.0;

    Eigen::Matrix3d prewitt_x, prewitt_y;

    prewitt_x << -1, 0, 1, -1, 0, 1, -1, 0, 1;

    prewitt_y << 1, 1, 1, 0, 0, 0, -1, -1, -1;

    setOperator(prewitt_x, prewitt_y);
}

Convolver* PrewittConvolver::clone() const
{
    return new PrewittConvolver(*this);
}
} // namespace ohkl
