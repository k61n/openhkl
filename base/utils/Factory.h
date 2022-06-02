//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Factory.h
//! @brief     Defines class Factory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_UTILS_FACTORY_H
#define NSX_BASE_UTILS_FACTORY_H

#include "base/utils/ISingleton.h"
#include <algorithm>
#include <functional>
#include <map>
#include <stdexcept>
#include <vector>

namespace nsx {

//! Generic factory class template.

//! Templated on the return type, the key and accepts any number
//! and/or type of parameters using variadic templates for the callback.

template <typename Child, typename returnType, typename keytype, typename... args>
class Factory : public ISingleton<Child> {
    typedef std::function<returnType*(args...)> callback;
    typedef std::map<keytype, callback> callbackmap;

 public:
    Factory() { }
    virtual ~Factory() { }

    //! register a new callback for constructing an object of type returnType
    //! @param key : key representing how the callback will be stored
    //! @param cb  : address of the function for the callback
    void registerCallback(const keytype& key, callback cb);
    //! remove all entries registered in the factory
    void clear();
    //! create dynamically an object
    //! @param key : key representing how the callback is stored
    //! @param arg : parameters pack
    returnType* create(const keytype& key, args... arg);
    //! return the keys of the object currently registered in the factory
    std::vector<keytype> list() const;
    //! remove a key from the factory
    //! @param key : key representing how the callback is stored
    std::size_t unregisterCallback(const keytype& key);
    //! Returns true if a given callback has been registered so far
    bool hasCallback(const keytype& key);

 protected:
    callbackmap _map;
};

template <typename Child, typename base, typename keytype, typename... args>
void Factory<Child, base, keytype, args...>::registerCallback(const keytype& key, callback cb)
{
    auto it = _map.find(key);
    if (it == _map.end())
        _map.insert(typename callbackmap::value_type(key, cb));
    else
        throw std::invalid_argument("Callback key already in use.");
}

template <typename Child, typename base, typename keytype, typename... args>
void Factory<Child, base, keytype, args...>::clear()
{
    _map.clear();
}

template <typename Child, typename base, typename keytype, typename... args>
base* Factory<Child, base, keytype, args...>::create(const keytype& key, args... arg)
{
    auto it = _map.find(key);
    if (it != _map.end())
        return ((it->second)(arg...));
    else
        throw std::invalid_argument("Factory error: callback not registered for " + key + " class");
}

template <typename Child, typename base, typename keytype, typename... args>
std::vector<keytype> Factory<Child, base, keytype, args...>::list() const
{
    std::vector<keytype> keys;
    keys.reserve(_map.size());
    std::for_each(_map.begin(), _map.end(), [&keys](const std::pair<keytype, callback>& it) {
        keys.push_back(it.first);
    });
    return keys;
}

template <typename Child, typename base, typename keytype, typename... args>
std::size_t Factory<Child, base, keytype, args...>::unregisterCallback(const keytype& key)
{
    return _map.erase(key);
}

template <typename Child, typename base, typename keytype, typename... args>
bool Factory<Child, base, keytype, args...>::hasCallback(const keytype& key)
{
    auto it = _map.find(key);
    return (it != _map.end());
}

} // namespace nsx

#endif // NSX_BASE_UTILS_FACTORY_H
