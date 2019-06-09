//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/MetaData.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "core/loader/MetaData.h"

namespace nsx {

// Static that contains all available keys.
MetaDataKeySet MetaData::_metakeys = std::set<std::string>();

MetaData::MetaData() {}

MetaData::~MetaData() {}

MetaData::MetaData(const MetaData& rhs)
{
    _map = rhs._map;
}

MetaData& MetaData::operator=(const MetaData& rhs)
{
    if (this != &rhs)
        _map = rhs._map;
    return *this;
}

const MetaDataKeySet& MetaData::keys() const
{
    return _metakeys;
}
bool MetaData::isKey(const char* key) const
{
    // Search if this key is in the set.
    auto it = _metakeys.find(std::string(key));
    if (it == _metakeys.end())
        return false;
    const char* ptr = (*it).c_str();
    auto it2 = _map.find(ptr);
    return (it2 != _map.end());
}
bool MetaData::isKey(const std::string& key) const
{
    // Search if this key is in the set.
    auto it = _metakeys.find(key);
    if (it == _metakeys.end())
        return false;
    const char* ptr = (*it).c_str();
    auto it2 = _map.find(ptr);
    return (it2 != _map.end());
}

Variant<int, double, std::string> MetaData::key(const std::string& key) const
{
    // Search if this key is in the set.
    auto it = _metakeys.find(key);
    if (it == _metakeys.end())
        throw std::runtime_error("Could not locate key " + key + " in the set of available keys");
    const char* ptr = (*it).c_str();
    auto it2 = _map.find(ptr);
    if (it2 != _map.end()) {
        return it2->second;
    } else
        throw std::runtime_error("Could not locate key " + key + " in the set of available keys");
}

const MetaDataMap& MetaData::map() const
{
    return _map;
}

} // End namespace nsx
