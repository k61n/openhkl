//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/EnhancedAnnularConvolver.h
//! @brief     Defines class EnhancedAnnularConvolver
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_CONVOLVE_ENHANCEDANNULARCONVOLVER_H
#define NSX_CORE_CONVOLVE_ENHANCEDANNULARCONVOLVER_H

#include "core/convolve/Convolver.h" // inherits from

namespace ohkl {

//! More sophisticated version of local background subtraction. TODO: analyse and document (#33)

//! TODO: Legacy description was the same as for AnnularConvolver and RadialConvolver.
//!       So most likely it was just copied and pasted and forgotten to update.
//!       What this convolver really does, we have to find out.

class EnhancedAnnularConvolver : public Convolver {
 public:
    EnhancedAnnularConvolver();

    EnhancedAnnularConvolver(const EnhancedAnnularConvolver& other) = default;

    EnhancedAnnularConvolver(const std::map<std::string, double>& parameters);

    ~EnhancedAnnularConvolver() = default;

    EnhancedAnnularConvolver& operator=(const EnhancedAnnularConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

    RealMatrix convolve(const RealMatrix& image) override;
};

} // namespace ohkl

#endif // NSX_CORE_CONVOLVE_ENHANCEDANNULARCONVOLVER_H
