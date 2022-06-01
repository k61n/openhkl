//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/HardwareParameters.h from core/resources/Resources.h.in
//! @brief     Declares functions that relate to resource files
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INSTRUMENT_HARDWAREPARAMETERS_H
#define NSX_CORE_INSTRUMENT_HARDWAREPARAMETERS_H

#include <yaml-cpp/yaml.h>

namespace nsx {

YAML::Node findResource(const std::string& instrumentName);

std::set<std::string> getResourcesName(const std::string&);

} // namespace nsx

#endif // NSX_CORE_INSTRUMENT_HARDWAREPARAMETERS_H
