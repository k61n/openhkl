//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/IntegratorFactory.h
//! @brief     Defines class IntegratorFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INTEGRATE_INTEGRATORFACTORY_H
#define OHKL_CORE_INTEGRATE_INTEGRATORFACTORY_H

#include "core/integration/IIntegrator.h"

namespace ohkl {

class IntegratorFactory {
 public:
    using callback = std::function<IIntegrator*()>;

    IntegratorFactory();
    IIntegrator* create(const IntegratorType& integrator_type) const;

    const std::map<IntegratorType, callback>& callbacks() const;

 private:
    std::map<IntegratorType, callback> _callbacks;
};

} // namespace ohkl

#endif // OHKL_CORE_INTEGRATE_INTEGRATORFACTORY_H
