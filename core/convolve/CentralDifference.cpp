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

namespace ohkl {

CentralDifference::CentralDifference() : GradientConvolver() { }

CentralDifference::CentralDifference(const std::map<std::string, double>& parameters)
    : GradientConvolver(parameters)
{
    _norm_fac = 0.5;

    Eigen::Matrix3d central_x, central_y;

    central_x <<  0,  0,  0,
                 -1,  0,  1,
                  0,  0,  0;

    central_y <<  0,  1,  0,
                  0,  0,  0,
                  0, -1,  0;

    setOperator(central_x, central_y);
}

Convolver* CentralDifference::clone() const
{
    return new CentralDifference(*this);
}
} // namespace ohkl
