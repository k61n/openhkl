//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/CentralDifferenceKernel.h
//! @brief     Defines class CentralDifferenceKernel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_CENTRALDIFFERENCEKERNEL_H
#define OHKL_CORE_CONVOLVE_CENTRALDIFFERENCEKERNEL_H

#include "core/convolve/ConvolutionKernel.h"
#include "core/convolve/GradientConvolutionKernel.h"

namespace ohkl {

//! Convolution kernel to compute an image gradient

    class CentralDifferenceKernel : public GradientConvolutionKernel {
 public:
    CentralDifferenceKernel();

    int kernelSize() const { return _size; };

 protected:
    int _size;

};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_CENTRALDIFFERENCEKERNEL_H
