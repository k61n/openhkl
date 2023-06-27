//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/HardwareParameters.h from core/resources/Resources.h.in
//! @brief     Declares functions that relate to resource files
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_HARDWAREPARAMETERS_H
#define OHKL_CORE_INSTRUMENT_HARDWAREPARAMETERS_H

#include <yaml-cpp/yaml.h>

#include <map>

namespace ohkl {

YAML::Node findResource(const std::string& instrumentName);

std::set<std::string> getInstrumentNames(const std::string&);

const std::map<std::string, std::string> instruments = {
    {"BioDiff", "BioDiff"},
    {"D9", "D9"},
    {"D9_lifting arm", "D9 lifting arm"},
    {"D9_large", "D9 large"},
    {"D9_large_lifting_arm", "D9 large lifting arm"},
    {"D10", "D10"},
    {"D19", "D19"},
    {"I16", "I16"},
};

} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_HARDWAREPARAMETERS_H
