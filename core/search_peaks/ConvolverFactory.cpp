//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/search_peaks/ConvolverFactory.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/search_peaks/ConvolverFactory.h"
#include "core/search_peaks/AnnularConvolver.h"
#include "core/search_peaks/ConstantConvolver.h"
#include "core/search_peaks/DeltaConvolver.h"
#include "core/search_peaks/EnhancedAnnularConvolver.h"
#include "core/search_peaks/RadialConvolver.h"

namespace nsx {

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
}

Convolver* ConvolverFactory::create(
    const std::string& convolver_type, const std::map<std::string, double>& parameters) const
{
    const auto it = _callbacks.find(convolver_type);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error(convolver_type + " is not registered as a valid convolver");
    }

    return (it->second)(parameters);
}

const std::map<std::string, ConvolverFactory::callback>& ConvolverFactory::callbacks() const
{
    return _callbacks;
}

} // end namespace nsx
