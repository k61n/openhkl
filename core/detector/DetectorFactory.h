//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/DetectorFactory.h
//! @brief     Defines class DetectorFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_DETECTOR_DETECTORFACTORY_H
#define NSX_CORE_DETECTOR_DETECTORFACTORY_H

#include "base/utils/Factory.h"

namespace nsx {

//! Factory that gives access to Detector%s of different geometries.

class DetectorFactory : public Factory<DetectorFactory, Detector, std::string, const YAML::Node&> {
 public:
    DetectorFactory();
};

} // namespace nsx

#endif // NSX_CORE_DETECTOR_DETECTORFACTORY_H
