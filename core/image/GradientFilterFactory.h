//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/GradientFilterFactory.h
//! @brief     Defines class GradientFilterFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_IMAGE_GRADIENTFILTERFACTORY_H
#define OHKL_CORE_IMAGE_GRADIENTFILTERFACTORY_H

#include <Eigen/Dense>

#include <functional>
#include <map>
#include <string>

namespace ohkl {

class GradientFilter;

//! Factory for the different convolvers (i.e. image filters).

class GradientFilterFactory {
 public:
    using callback = std::function<GradientFilter*()>;

    GradientFilterFactory();

    GradientFilter* create(const std::string& filter_type) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string, callback>& callbacks() const;

    ~GradientFilterFactory() = default;

 private:
    std::map<std::string, callback> _callbacks;
};


} // namespace ohkl

#endif // OHKL_CORE_IMAGE_GRADIENTFILTERFACTORY_H
