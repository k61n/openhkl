//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/HardwareParameters.cpp from core/resources/Resources.cpp.in
//! @brief     Defines functions that relate to resource files
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/HardwareParameters.h"
#include <stdexcept>

namespace ohkl {

YAML::Node findResource(const std::string& instrumentName)
{
    if (!instruments.count(instrumentName))
        throw std::runtime_error("findResource: unable to find instrument " + instrumentName);
    std::string path = "data/instruments/" + instrumentName + ".yml";
    return YAML::LoadFile(path);
}

std::set<std::string> getInstrumentNames(const std::string& /* resourceType */)
{
    std::set<std::string> ret;
    for (auto it = instruments.begin(); it != instruments.end(); ++it)
        ret.insert(it->first);
    return ret;
}

} // namespace ohkl
