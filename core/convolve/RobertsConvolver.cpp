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
#include "core/convolve/GradientConvolver.h"

namespace ohkl {

RobertsConvolver::RobertsConvolver() : GradientConvolver() { }

RobertsConvolver::RobertsConvolver(const std::map<std::string, double>& parameters)
    : GradientConvolver(parameters)
{
    _norm_fac = 0.5;

    Eigen::Matrix2d roberts_x, roberts_y;

    roberts_x <<  1,  0,
                  0, -1;

    roberts_y <<  0,  1,
                 -1,  0;

    setOperator(roberts_x, roberts_y);
}

Convolver* RobertsConvolver::clone() const
{
    return new RobertsConvolver(*this);
}
} // namespace ohkl
