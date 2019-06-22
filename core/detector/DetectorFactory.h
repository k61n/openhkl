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

#ifndef CORE_DETECTOR_DETECTORFACTORY_H
#define CORE_DETECTOR_DETECTORFACTORY_H

#include "base/utils/Factory.h"
#include "base/utils/Singleton.h"

namespace nsx {

//! Factory that gives access to `Detector`s of different geometries.

class DetectorFactory : public Factory<Detector, std::string, const YAML::Node&>,
                        public Singleton<DetectorFactory, Constructor, Destructor> {
 private:
    friend class Constructor<DetectorFactory>;
    friend class Destructor<DetectorFactory>;
    DetectorFactory();
    ~DetectorFactory();
};

} // namespace nsx

#endif // CORE_DETECTOR_DETECTORFACTORY_H
