//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/DeltaConvolver.h
//! @brief     Defines class DeltaConvolver
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_CONVOLVE_DELTACONVOLVER_H
#define NSX_CORE_CONVOLVE_DELTACONVOLVER_H

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Dirac-delta kernel.

class DeltaConvolver : public AtomicConvolver {
 public:
    DeltaConvolver();

    DeltaConvolver(const DeltaConvolver& other) = default;

    DeltaConvolver(const std::map<std::string, double>& parameters);

    ~DeltaConvolver() = default;

    DeltaConvolver& operator=(const DeltaConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

 private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // namespace ohkl

#endif // NSX_CORE_CONVOLVE_DELTACONVOLVER_H
