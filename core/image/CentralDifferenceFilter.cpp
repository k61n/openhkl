//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/image/CentralDifferenceFilter.cpp
//! @brief     Implements class CentralDifferenceFilter
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/CentralDifferenceFilter.h"

#include "core/convolve/CentralDifferenceKernel.h"

#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

namespace ohkl {

CentralDifferenceFilter::CentralDifferenceFilter() : GradientFilter()
{
    CentralDifferenceKernel kernel;
    _kernel_x = kernel.matrixX();
    _kernel_y = kernel.matrixY();
}



} // namespace ohkl
