//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/GradientFilterFactory.cpp
//! @brief     Implements class GradientFilterFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/GradientFilterFactory.h"

#include "core/image/CentralDifferenceFilter.h"
#include "core/image/Sobel3Filter.h"

namespace ohkl {

template <typename T> T* create_filter()
{
    return new T();
}

GradientFilterFactory::GradientFilterFactory() : _callbacks()
{
    _callbacks["Central difference"] = &create_filter<CentralDifferenceFilter>;
    _callbacks["Sobel 3"] = &create_filter<Sobel3Filter>;
}

GradientFilter* GradientFilterFactory::create(const std::string& filter_type) const
{
    const auto it = _callbacks.find(filter_type);

    // could not find key
    if (it == _callbacks.end())
        throw std::runtime_error(filter_type + " is not registered as a valid gradient filter");

    GradientFilter* temp = (it->second)();
    return temp;
}

const std::map<std::string, GradientFilterFactory::callback>& GradientFilterFactory::callbacks() const
{
    return _callbacks;
}

} // namespace ohkl
