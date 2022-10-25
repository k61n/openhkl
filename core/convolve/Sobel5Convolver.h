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

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Sobel5 gradient kernel
//!
//! dx = [  1  0 -1 ] dy = [  1  2  1 ]
//!      [  2  0 -2 ]      [  0  0  0 ]
//!      [  1  0 -1 ]      [ -1 -2 -1 ]
//!
//! Computes a smoothed central difference gradient

class Sobel5Convolver : public AtomicConvolver {
 public:
    Sobel5Convolver();

    Sobel5Convolver(const Sobel5Convolver& other) = default;

    Sobel5Convolver(const std::map<std::string, double>& parameters);

    ~Sobel5Convolver() = default;

    Sobel5Convolver& operator=(const Sobel5Convolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

 private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_SOBEL5CONVOLVER_H
