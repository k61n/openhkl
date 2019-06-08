//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/axes/AxisFactory.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_AXES_AXISFACTORY_H
#define CORE_AXES_AXISFACTORY_H

#include <string>

#include <yaml-cpp/yaml.h>

#include "core/axes/Axis.h"
#include "core/utils/Factory.h"
#include "core/utils/Singleton.h"

namespace nsx {

//! Factory class used to create axes (rotational, translational)
class AxisFactory : public Factory<Axis, std::string, const YAML::Node&>,
                    public Singleton<AxisFactory, Constructor, Destructor> {
private:
    friend class Constructor<AxisFactory>;
    friend class Destructor<AxisFactory>;
    AxisFactory();
    ~AxisFactory();
};

} // end namespace nsx

#endif // CORE_AXES_AXISFACTORY_H
