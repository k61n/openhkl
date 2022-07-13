//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/ConvolverFactory.h
//! @brief     Defines class ConvolverFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_CONVOLVE_CONVOLVERFACTORY_H
#define NSX_CORE_CONVOLVE_CONVOLVERFACTORY_H

#include "core/convolve/Convolver.h"

namespace ohkl {

//! Factory for the different convolvers (i.e. image filters).

class ConvolverFactory {
 public:
    using callback = std::function<Convolver*(const std::map<std::string, double>&)>;

    ConvolverFactory();

    Convolver* create(
        const std::string& convolver_type, const std::map<std::string, double>& parameters) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string, callback>& callbacks() const;

    ~ConvolverFactory() = default;

 private:
    std::map<std::string, callback> _callbacks;
};

//! Returns a convolved frame
Eigen::MatrixXd convolvedFrame(
    Eigen::MatrixXi frame_data, const std::string& convolver_type,
    const std::map<std::string, double>& parameters);

} // namespace ohkl

#endif // NSX_CORE_CONVOLVE_CONVOLVERFACTORY_H
