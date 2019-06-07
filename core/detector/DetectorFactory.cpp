//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/detector/DetectorFactory.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>
#include <string>

#include "CylindricalDetector.h"
#include "DetectorFactory.h"
#include "FlatDetector.h"

namespace nsx {

DetectorFactory::DetectorFactory()
{
    registerCallback("flat", &FlatDetector::create);
    registerCallback("cylindrical", &CylindricalDetector::create);
}

DetectorFactory::~DetectorFactory() {}

} // end namespace nsx
