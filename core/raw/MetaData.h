//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/raw/MetaData.h
//! @brief     Defines class MetaData
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_RAW_METADATA_H
#define OHKL_CORE_RAW_METADATA_H

#include "base/utils/Logger.h"
#include "core/raw/DataKeys.h"

#include <map>
#include <set>
#include <variant>

namespace ohkl {

using MetaDataMap = std::map<std::string, std::variant<int, double, std::string>>;
using MetaDataKeySet = std::set<std::string>;

//! Stores arbitrary meta data of a DataSet.

//! MetaData class allow to store metadata associated with a data file  into a
//! map indexed by the string of the entry. Any parameter type can be stored,
//! using the corresponding template argument. A specific key can be retrieved
//! back using the template type defined when the key was registered.
class MetaData {
 public:
    //! Constructor
    MetaData() = default;

    //! Add a pair of key and value.
    template <class _type> void add(const std::string& key, const _type& value);
    //! Gets the value associated with the key. User must know the return type,
    //! otherwise use method that return boost_any
    template <class _type> _type key(const std::string& key) const;
    //! Gets the value associated with the key. User must know the return type,
    //! otherwise use method that return boost_any
    template <class _type> _type key(const char* key) const;
    //! Returns the value
    //@ return : value corresponding to key
    std::variant<int, double, std::string> key(const std::string& key) const;
    //! Is this key in the metadata
    bool isKey(const std::string& key) const;
    //! Is this key in the metadata
    bool isKey(const char* key) const;
    //! Number of elements in the map
    //@ return : Number of elements in the map
    // std::size_t size() const;
    //! Gets all the keys available.
    const MetaDataKeySet& keys() const;
    //! Gets the full map of parameters
    const MetaDataMap& map() const;
    //! Overwrites the full map of parameters
    void setMap(const MetaDataMap& other_map);
    //! Merges the map of parameters with another given map
    void addMap(const MetaDataMap& other_map);

 private:
    //! Contains the map of all key/value pairs.
    MetaDataMap _map;
    //! Contains all available keys so far.
    static MetaDataKeySet _metakeys;

    friend std::ostream& operator<<(std::ostream& os, const MetaData& metadata);
};

template <typename _type> void MetaData::add(const std::string& key, const _type& value)
{
    // Warn against unrecognized keys
    if (ohkl::RecognizedMetaDataKeys.count(key) == 0) {
        ohklLog(ohkl::Level::Warning, __FUNCTION__, ": MetaData key '" + key + "' not recognized.");
    }

    // First, make sure the key is already in the keyset
    // Since this is a set, no duplicate will be found. No need to search
    // explicitly.
    _metakeys.insert(key);

    //  If all OK, then add the key to the map
    _map.insert_or_assign(key, value);
}

template <typename _type> _type MetaData::key(const std::string& name) const
{
    // Search if this key is in the set.
    auto it = _metakeys.find(name);
    if (it == _metakeys.end())
        throw std::runtime_error("Could not find key '" + name + "' in MetaData");

    // Then search in the map
    auto it2 = _map.find(name);
    if (it2 == _map.end())
        throw std::runtime_error("Could not find key '" + name + "' in MetaData");
    return std::get<_type>(it2->second);
}

template <typename _type> _type MetaData::key(const char* name) const
{
    return key<_type>(std::string(name));
}

} // namespace ohkl

#endif // OHKL_CORE_RAW_METADATA_H
