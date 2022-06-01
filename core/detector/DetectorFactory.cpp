//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/detector/DetectorFactory.cpp
//! @brief     Implements class DetectorFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>
#include <string>

#include "core/detector/CylindricalDetector.h"
#include "core/detector/DetectorFactory.h"
#include "core/detector/FlatDetector.h"

namespace nsx {

DetectorFactory::DetectorFactory()
{
    registerCallback("flat", &FlatDetector::create);
    registerCallback("cylindrical", &CylindricalDetector::create);
}

} // namespace nsx
