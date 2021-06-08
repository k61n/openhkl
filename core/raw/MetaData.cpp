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

void MetaData::setMap(const MetaDataMap& other_map)
{
    _map = other_map;
}

void MetaData::addMap(const MetaDataMap& other_map)
{
    _map.insert(other_map.begin(), other_map.end());
}

std::ostream& operator<<(std::ostream& os, const MetaData& metadata)
{
    os << "MetaData {";
    for (const auto& [key, val] : metadata._map) {
        os << key;
        if (std::holds_alternative<int>(val))
            os << "<int>: " << std::get<int>(val);
        else if (std::holds_alternative<double>(val))
            os << "<double>: " << std::get<double>(val);
        else if (std::holds_alternative<std::string>(val))
            os << "<string>: " << std::get<std::string>(val);
        else
            os << "<?>: ?";

        os << ", ";
    }
    os << "}";
    return os;
}

} // End namespace nsx
