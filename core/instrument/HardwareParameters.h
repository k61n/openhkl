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

#ifndef OHKL_CORE_INSTRUMENT_HARDWAREPARAMETERS_H
#define OHKL_CORE_INSTRUMENT_HARDWAREPARAMETERS_H

#include <yaml-cpp/yaml.h>

namespace ohkl {

YAML::Node findResource(const std::string& instrumentName);

std::set<std::string> getResourcesName(const std::string&);

} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_HARDWAREPARAMETERS_H
