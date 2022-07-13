//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/ConstantConvolver.h
//! @brief     Defines class ConstantConvolver
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_CONSTANTCONVOLVER_H
#define OHKL_CORE_CONVOLVE_CONSTANTCONVOLVER_H

#include "core/convolve/AtomicConvolver.h" // inherits from

namespace ohkl {

//! Convolver with constant kernel, to take the average value of the frame.

//! The kernel has the constant value 1 / (rows*cols).

class ConstantConvolver : public AtomicConvolver {
 public:
    ConstantConvolver();

    ConstantConvolver(const ConstantConvolver& other) = default;

    ConstantConvolver(const std::map<std::string, double>& parameters);

    ~ConstantConvolver() = default;

    ConstantConvolver& operator=(const ConstantConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

 private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_CONSTANTCONVOLVER_H
