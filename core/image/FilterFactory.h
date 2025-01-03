//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/FilterFactory.h
//! @brief     Defines class FilterFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_FILTERFACTORY_H
#define OHKL_CORE_IMAGE_FILTERFACTORY_H

#include <Eigen/Dense>

#include <functional>
#include <map>
#include <string>

namespace ohkl {

class ImageFilter;

//! Factory for the different convolvers (i.e. image filters).

class FilterFactory {
 public:
    using callback = std::function<ImageFilter*(const std::map<std::string, double>&)>;

    FilterFactory();

    ImageFilter* create(
        const std::string& filter_type, const std::map<std::string, double>& parameters) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string, callback>& callbacks() const;

    ~FilterFactory() = default;

 private:
    std::map<std::string, callback> _callbacks;
};


} // namespace ohkl

#endif // OHKL_CORE_IMAGE_FILTERFACTORY_H
