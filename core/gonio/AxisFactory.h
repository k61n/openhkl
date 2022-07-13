//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/gonio/AxisFactory.h
//! @brief     Defines class AxisFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_GONIO_AXISFACTORY_H
#define NSX_CORE_GONIO_AXISFACTORY_H

#include "base/utils/Factory.h"
#include "base/utils/ISingleton.h"
#include "core/gonio/Axis.h"

namespace ohkl {

//! Factory to create axes (rotational, translational).

class AxisFactory : public Factory<AxisFactory, Axis, std::string, const YAML::Node&> {
 public:
    AxisFactory();
};

} // namespace ohkl

#endif // NSX_CORE_GONIO_AXISFACTORY_H
