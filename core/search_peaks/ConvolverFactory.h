//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/search_peaks/ConvolverFactory.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_SEARCH_PEAKS_CONVOLVERFACTORY_H
#define CORE_SEARCH_PEAKS_CONVOLVERFACTORY_H

#include <functional>

#include "core/search_peaks/Convolver.h"

namespace nsx {

//! \brief Factory class for the different image filters.
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

} // namespace nsx

#endif // CORE_SEARCH_PEAKS_CONVOLVERFACTORY_H
