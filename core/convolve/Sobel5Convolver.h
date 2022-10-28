//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/Sobel5Convolver.h
//! @brief     Defines class Sobel5Convolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_SOBEL5CONVOLVER_H
#define OHKL_CORE_CONVOLVE_SOBEL5CONVOLVER_H

#include "core/convolve/GradientConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Sobel 5x5 gradient kernel
//!
//! dx = [ -5  -4    0   4    5  ] dy = [   5   8   10  8    5  ]
//!      [ -8  -10   0   10   8  ]      [   4   10  20  10   4  ]
//!      [ -10 -20   0   20   10 ]      [   0   0   0   0    0  ]
//!      [ -8  -10   0   10   8  ]      [  -4  -10 -20 -10  -4  ]
//!      [ -5  -4    0   4    5  ]      [  -5  -8  -10 -8   -5  ]
//!
//! Computes a smoothed central difference gradient

class Sobel5Convolver : public GradientConvolver {
 public:
    Sobel5Convolver();

    Sobel5Convolver(const Sobel5Convolver& other) = default;

    Sobel5Convolver(const std::map<std::string, double>& parameters);

    ~Sobel5Convolver() = default;

    Sobel5Convolver& operator=(const Sobel5Convolver& other) = default;

    Convolver* clone() const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_SOBEL5CONVOLVER_H
