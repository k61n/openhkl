//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/HardwareParameters.cpp from core/resources/Resources.cpp.in
//! @brief     Defines functions that relate to resource files
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/HardwareParameters.h"

#include <map>

std::map<std::string, const char*> database = {
    {
        "BioDiff2500",
#include "data/instruments/BioDiff2500.yml2c"
    },
    {
        "BioDiff5000",
#include "data/instruments/BioDiff5000.yml2c"
    },
    {
        "D9",
#include "data/instruments/D9.yml2c"
    },
    {
        "D10",
#include "data/instruments/D10.yml2c"
    },
    {
        "D19",
#include "data/instruments/D19.yml2c"
    },
    {
        "D9_large",
#include "data/instruments/D9_large.yml2c"
    },
    {
        "D9_lifting_arm",
#include "data/instruments/D9_lifting_arm.yml2c"
    },
    {
        "D9_large_lifting_arm",
#include "data/instruments/D9_large_lifting_arm.yml2c"
    },
    {
        "I16",
#include "data/instruments/I16.yml2c"
    },
};

namespace nsx {

YAML::Node findResource(const std::string& instrumentName)
{
    return YAML::Load(database.at(instrumentName));
}

std::set<std::string> getResourcesName(const std::string& resource_type)
{
    // assert(resource_type=="instrument");
    std::set<std::string> ret;
    for (auto it = database.begin(); it != database.end(); ++it)
        ret.insert(it->first);
    return ret;
}

} // namespace nsx
