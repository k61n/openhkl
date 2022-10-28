//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/RobertsConvolver.h
//! @brief     Defines class RobertsConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_ROBERTSCONVOLVER_H
#define OHKL_CORE_CONVOLVE_ROBERTSCONVOLVER_H

#include "core/convolve/GradientConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Roberts gradient kernel
//!
//! dx = [  1  0 ] dy = [  0  1 ]
//!      [  0 -1 ]      [ -1  0 ]
//!
//!
//!

class RobertsConvolver : public GradientConvolver {
 public:
    RobertsConvolver();

    RobertsConvolver(const RobertsConvolver& other) = default;

    RobertsConvolver(const std::map<std::string, double>& parameters);

    ~RobertsConvolver() = default;

    RobertsConvolver& operator=(const RobertsConvolver& other) = default;

    Convolver* clone() const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_ROBERTSCONVOLVER_H
