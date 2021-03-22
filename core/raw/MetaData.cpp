//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/raw/MetaData.cpp
//! @brief     Implements class MetaData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/raw/MetaData.h"

namespace nsx {

// Static that contains all available keys.
MetaDataKeySet MetaData::_metakeys = std::set<std::string>();

const MetaDataKeySet& MetaData::keys() const
{
    return _metakeys;
}

bool MetaData::isKey(const char* key) const
{
    return isKey(std::string(key));
}

bool MetaData::isKey(const std::string& key) const
{
    // Search if this key is in the set.
    auto it = _metakeys.find(key);
    if (it == _metakeys.end())
        return false;

    auto it2 = _map.find(key);
    return it2 != _map.end();
}

std::variant<int, double, std::string> MetaData::key(const std::string& key) const
{
    // Search if this key is in the set.
    auto it = _metakeys.find(key);
    if (it == _metakeys.end())
        throw std::runtime_error("Could not locate key " + key + " in the set of available keys");

    auto it2 = _map.find(key);
    if (it2 == _map.end())
        throw std::runtime_error("Could not locate key " + key + " in the set of available keys");
    return it2->second;
}

const MetaDataMap& MetaData::map() const
{
    return _map;
}
} // End namespace nsx
