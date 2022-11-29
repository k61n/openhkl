//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/Convolver.cpp
//! @brief     Implements class Convolver
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/Convolver.h"

namespace ohkl {

using parameters_map = std::map<std::string, double>;

const std::map<ConvolutionKernelType, std::string> Convolver::kernelTypes{
    {ConvolutionKernelType::None, "none"},
    {ConvolutionKernelType::Annular, "annular"},
    {ConvolutionKernelType::EnhancedAnnular, "enhanced annular"},
    {ConvolutionKernelType::Radial, "radial"},
    {ConvolutionKernelType::Constant, "constant"},
    {ConvolutionKernelType::Delta, "delta"}};

Convolver::Convolver() : _parameters() { }

Convolver::Convolver(const parameters_map& parameters)
{
    _parameters = parameters;
}

Convolver::~Convolver() = default;

std::map<std::string, double>& Convolver::parameters()
{
    return _parameters;
}

const std::map<std::string, double>& Convolver::parameters() const
{
    return _parameters;
}

void Convolver::setParameters(const std::map<std::string, double>& parameters)
{
    for (const auto& p : parameters) {
        auto it = _parameters.find(p.first);
        if (it != _parameters.end())
            it->second = p.second;
    }
}

} // namespace ohkl
