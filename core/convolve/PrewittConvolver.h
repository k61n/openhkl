//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/PrewittConvolver.h
//! @brief     Defines class PrewittConvolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_CONVOLVE_PREWITTCONVOLVER_H
#define OHKL_CORE_CONVOLVE_PREWITTCONVOLVER_H

#include "core/convolve/GradientConvolver.h" // inherits from

namespace ohkl {

//! Convolver with Prewitt gradient kernel
//!
//! dx = [ -1  0  1 ] dy = [  1  1  1 ]
//!      [ -1  0  1 ]      [  0  0  0 ]
//!      [ -1  0  1 ]      [ -1 -1 -1 ]
//!
//! Computes an averaged central difference gradient

class PrewittConvolver : public GradientConvolver {
 public:
    PrewittConvolver();

    PrewittConvolver(const PrewittConvolver& other) = default;

    PrewittConvolver(const std::map<std::string, double>& parameters);

    ~PrewittConvolver() = default;

    PrewittConvolver& operator=(const PrewittConvolver& other) = default;

    Convolver* clone() const override;
};

} // namespace ohkl

#endif // OHKL_CORE_CONVOLVE_PREWITTCONVOLVER_H
