//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/convolve/EnhancedAnnularConvolver.cpp
//! @brief     Implements class EnhancedAnnularConvolver
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>

#include "core/convolve/EnhancedAnnularConvolver.h"
#include "core/convolve/RadialConvolver.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

// M_PI is often defined but not standard
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884
#endif

namespace nsx {

EnhancedAnnularConvolver::EnhancedAnnularConvolver()
    : Convolver({{"r1", 5}, {"r2", 10}, {"r3", 15}})
{
}

EnhancedAnnularConvolver::EnhancedAnnularConvolver(const std::map<std::string, double>& parameters)
    : EnhancedAnnularConvolver()
{
    setParameters(parameters);
}

Convolver* EnhancedAnnularConvolver::clone() const
{
    return new EnhancedAnnularConvolver(*this);
}

std::pair<size_t, size_t> EnhancedAnnularConvolver::kernelSize() const
{
    size_t r = _parameters.at("r3");

    return std::make_pair(r, r);
}

RealMatrix EnhancedAnnularConvolver::convolve(const RealMatrix& image)
{
    RadialConvolver radial_convolver_peak({{"r_in", 0.0}, {"r_out", _parameters.at("r1")}});
    RealMatrix conv_peak = radial_convolver_peak.convolve(image);

    RadialConvolver radial_convolver_bkg(
        {{"r_in", _parameters.at("r2")}, {"r_out", _parameters.at("r3")}});
    RealMatrix bkg = radial_convolver_bkg.convolve(image);

    RealMatrix diff2 = (image - bkg).cwiseProduct(image - bkg);
    RealMatrix std = radial_convolver_bkg.convolve(diff2).array().sqrt();

    RealMatrix result = (conv_peak - bkg).array() / std.array();

    return result;
}

} // namespace nsx
