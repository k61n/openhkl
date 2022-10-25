//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/ConvolverFactory.cpp
//! @brief     Implements class ConvolverFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/ConvolverFactory.h"
#include "core/convolve/AnnularConvolver.h"
#include "core/convolve/ConstantConvolver.h"
#include "core/convolve/DeltaConvolver.h"
#include "core/convolve/EnhancedAnnularConvolver.h"
#include "core/convolve/RadialConvolver.h"
#include "core/convolve/SobelConvolver.h"

namespace ohkl {

template <typename T> T* create_convolver(const std::map<std::string, double>& parameters)
{
    return new T(parameters);
}

ConvolverFactory::ConvolverFactory() : _callbacks()
{
    _callbacks["annular"] = &create_convolver<AnnularConvolver>;
    _callbacks["constant"] = &create_convolver<ConstantConvolver>;
    _callbacks["delta"] = &create_convolver<DeltaConvolver>;
    _callbacks["enhanced annular"] = &create_convolver<EnhancedAnnularConvolver>;
    _callbacks["none"] = &create_convolver<DeltaConvolver>;
    _callbacks["radial"] = &create_convolver<RadialConvolver>;
    _callbacks["sobel"] = &create_convolver<SobelConvolver>;
}

Convolver* ConvolverFactory::create(
    const std::string& convolver_type, const std::map<std::string, double>& parameters) const
{
    const auto it = _callbacks.find(convolver_type);

    // could not find key
    if (it == _callbacks.end())
        throw std::runtime_error(convolver_type + " is not registered as a valid convolver");

    Convolver* temp = (it->second)(parameters);
    temp->setType(it->first);
    return temp;
}

const std::map<std::string, ConvolverFactory::callback>& ConvolverFactory::callbacks() const
{
    return _callbacks;
}

Eigen::MatrixXd convolvedFrame(
    Eigen::MatrixXi frame_data, const std::string& convolver_type,
    const std::map<std::string, double>& parameters)
{
    ConvolverFactory convolver_factory;
    auto convolver = convolver_factory.create(convolver_type, parameters);
    return convolver->convolve(frame_data.cast<double>());
}

} // namespace ohkl
