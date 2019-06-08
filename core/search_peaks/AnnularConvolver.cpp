//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/search_peaks/AnnularConvolver.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/search_peaks/AnnularConvolver.h"
#include "core/search_peaks/RadialConvolver.h"

namespace nsx {

AnnularConvolver::AnnularConvolver() : Convolver({{"r1", 5}, {"r2", 10}, {"r3", 15}}) {}

AnnularConvolver::AnnularConvolver(const std::map<std::string, double>& parameters)
    : AnnularConvolver()
{
    setParameters(parameters);
}

Convolver* AnnularConvolver::clone() const
{
    return new AnnularConvolver(*this);
}

std::pair<size_t, size_t> AnnularConvolver::kernelSize() const
{
    size_t r = _parameters.at("r3");

    return std::make_pair(r, r);
}

RealMatrix AnnularConvolver::convolve(const RealMatrix& image)
{
    RadialConvolver radial_convolver_peak({{"r_in", 0.0}, {"r_out", _parameters.at("r1")}});
    RealMatrix conv_peak = radial_convolver_peak.convolve(image);

    RadialConvolver radial_convolver_bkg(
        {{"r_in", _parameters.at("r2")}, {"r_out", _parameters.at("r3")}});
    RealMatrix conv_bkg = radial_convolver_bkg.convolve(image);

    RealMatrix result = conv_peak - conv_bkg;

    return result;
}

} // end namespace nsx
