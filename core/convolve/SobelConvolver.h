//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/SobelConvolver.h
//! @brief     Defines class SobelConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_SOBELCONVOLVER_H
#define OHKL_CORE_CONVOLVE_SOBELCONVOLVER_H

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Sobel gradient kernel
//!
//! dx = [ -1  0  1 ] dy = [  1  2  1 ]
//!      [ -2  0  2 ]      [  0  0  0 ]
//!      [ -1  0  1 ]      [ -1 -2 -1 ]
//!
//! Computes a smoothed central difference gradient

class SobelConvolver : public AtomicConvolver {
 public:
    SobelConvolver();

    SobelConvolver(const SobelConvolver& other) = default;

    SobelConvolver(const std::map<std::string, double>& parameters);

    ~SobelConvolver() = default;

    SobelConvolver& operator=(const SobelConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

 private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_SOBELCONVOLVER_H
