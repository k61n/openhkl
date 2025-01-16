//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/FilterFactory.cpp
//! @brief     Implements class FilterFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/FilterFactory.h"

#include "core/image/AnnularImageFilter.h"
#include "core/image/EnhancedAnnularImageFilter.h"

namespace ohkl {

template <typename T> T* create_filter(const std::map<std::string, double>& parameters)
{
    return new T(parameters);
}

FilterFactory::FilterFactory() : _callbacks()
{
    _callbacks["Annular"] = &create_filter<AnnularImageFilter>;
    _callbacks["Enhanced annular"] = &create_filter<EnhancedAnnularImageFilter>;
}

ImageFilter* FilterFactory::create(
    const std::string& filter_type, const std::map<std::string, double>& parameters) const
{
    const auto it = _callbacks.find(filter_type);

    // could not find key
    if (it == _callbacks.end())
        throw std::runtime_error(filter_type + " is not registered as a valid filter");

    ImageFilter* temp = (it->second)(parameters);
    return temp;
}

const std::map<std::string, FilterFactory::callback>& FilterFactory::callbacks() const
{
    return _callbacks;
}

} // namespace ohkl
