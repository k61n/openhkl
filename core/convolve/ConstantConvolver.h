//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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

#ifndef CORE_CONVOLVE_CONSTANTCONVOLVER_H
#define CORE_CONVOLVE_CONSTANTCONVOLVER_H

// Convolver description: this kernel is constant with the value 1 /
// (rows*cols). Convolving with this kernel has the effect of taking the average
// value of the frame.

#include "core/convolve/AtomicConvolver.h"

namespace nsx {

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

} // namespace nsx

#endif // CORE_CONVOLVE_CONSTANTCONVOLVER_H
