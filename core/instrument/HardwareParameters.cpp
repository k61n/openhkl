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

#include <map>

namespace {

std::map<std::string, const char*> database = {
    {
        "BioDiff",
#include "data/instruments/BioDiff.yml2c"
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


} // namespace

namespace ohkl {

YAML::Node findResource(const std::string& instrumentName)
{
    std::string path = "data/instruments/" + instrumentName + ".yml";
    return YAML::LoadFile(path);
}

std::set<std::string> getResourcesName(const std::string& /* resourceType */)
{
    std::set<std::string> ret;
    for (auto it = database.begin(); it != database.end(); ++it)
        ret.insert(it->first);
    return ret;
}

} // namespace ohkl
