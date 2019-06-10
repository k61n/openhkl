//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/DetectorFactory.h
//! @brief     Defines ###THINGS###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_DETECTOR_DETECTORFACTORY_H
#define CORE_DETECTOR_DETECTORFACTORY_H

#include "core/utils/Factory.h"
#include "core/utils/Singleton.h"

namespace nsx {

//! Factory class for producing different detector geometries.
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
