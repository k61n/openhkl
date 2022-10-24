//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/SobelConvolverX.h
//! @brief     Defines class SobelConvolverX
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_SOBELCONVOLVERX_H
#define OHKL_CORE_CONVOLVE_SOBELCONVOLVERX_H

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Dirac-delta kernel.

class SobelConvolverX : public AtomicConvolver {
 public:
    SobelConvolverX();

    SobelConvolverX(const SobelConvolverX& other) = default;

    SobelConvolverX(const std::map<std::string, double>& parameters);

    ~SobelConvolverX() = default;

    SobelConvolverX& operator=(const SobelConvolverX& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

 private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_SOBELCONVOLVERX_H
