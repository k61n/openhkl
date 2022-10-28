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

namespace ohkl {

Sobel5Convolver::Sobel5Convolver() : GradientConvolver() { }

Sobel5Convolver::Sobel5Convolver(const std::map<std::string, double>& parameters)
    : GradientConvolver(parameters)
{
    _norm_fac = 1.0 / 20.0;

    Eigen::MatrixXd sobel_x(5, 5), sobel_y(5, 5);

    sobel_x <<  -5,  -4,   0,   4,   5,
                -8, -10,   0,  10,   8,
               -10, -20,   0,  20,  10,
                -8, -10,   0,  10,   8,
                -5,  -4,   0,   4,   5;

    sobel_y <<   5,   8,  10,   8,   5,
                 4,  10,  20,  10,   4,
                 0,   0,   0,   0,   0,
                -4, -10, -20, -10,  -4,
                -5,  -8, -10,  -8,  -5;

    setOperator(sobel_x, sobel_y);
}

Convolver* Sobel5Convolver::clone() const
{
    return new Sobel5Convolver(*this);
}

} // namespace ohkl
