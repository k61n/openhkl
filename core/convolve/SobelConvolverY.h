//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/SobelConvolverY.h
//! @brief     Defines class SobelConvolverY
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_SOBELCONVOLVERY_H
#define OHKL_CORE_CONVOLVE_SOBELCONVOLVERY_H

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Dirac-delta kernel.

class SobelConvolverY : public AtomicConvolver {
 public:
    SobelConvolverY();

    SobelConvolverY(const SobelConvolverY& other) = default;

    SobelConvolverY(const std::map<std::string, double>& parameters);

    ~SobelConvolverY() = default;

    SobelConvolverY& operator=(const SobelConvolverY& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

 private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_SOBELCONVOLVERY_H
