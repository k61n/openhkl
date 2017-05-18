/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
	BP 156
	6, rue Jules Horowitz
	38042 Grenoble Cedex 9
	France
	chapon[at]ill.fr
    pellegrini[at]ill.fr

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef NSXTOOL_SHAREDFACTORY_H_
#define NSXTOOL_SHAREDFACTORY_H_

#include <algorithm>
#include <memory>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <map>
#include <vector>

namespace nsx {

/**
 * @brief generic SharedFactory class templated on the return type, the key and accepts any number
 * and/or type of parameters using variadic templates for the callback.
 */
template <typename returnType, typename keytype, typename ...args>
class SharedFactory {
	typedef std::function<returnType*(args...)> callback;
	typedef std::map<keytype,callback> callbackmap;

public:
    SharedFactory(){}
    virtual ~SharedFactory(){}

	//! register a new callback for constructing an object of type returnType
	//@param key : key representing how the callback will be stored
	//@param cb  : address of the function for the callback
	void registerCallback(const keytype& key,callback cb);
    //! remove all entries registered in the SharedFactory
	void clear();
	//! create dynamically an object
	//@param key : key representing how the callback is stored
	//@param arg : parameters pack
	returnType* create(const keytype& key, args... arg);
    //! return the keys of the object currently registered in the SharedFactory
	std::vector<keytype> list() const;
    //! remove a key from the SharedFactory
	//@param key : key representing how the callback is stored
	std::size_t unregisterCallback(const keytype& key);

protected:
	callbackmap _map;

};

template <typename base, typename keytype, typename ...args>
void SharedFactory<base,keytype,args...>::registerCallback(const keytype& key, callback cb)
{
	auto it=_map.find(key);
	if (it == _map.end())
		_map.insert(typename callbackmap::value_type(key,cb));
	else
		throw std::invalid_argument("Callback key already in use.");
}

template <typename base, typename keytype, typename ...args>
void SharedFactory<base,keytype,args...>::clear()
{
	_map.clear();
}

template <typename base, typename keytype, typename ...args>
base* SharedFactory<base,keytype,args...>::create(const keytype& key, args...arg)
{
	auto it=_map.find(key);
	if (it != _map.end())
		return ((it->second)(arg...));
	else
        throw std::invalid_argument("SharedFactory error: callback not registered for this class");
}

template <typename base, typename keytype, typename ...args>
std::vector<keytype> SharedFactory<base,keytype,args...>::list() const
{
	std::vector<keytype> keys;
	keys.reserve(_map.size());
	std::for_each(_map.begin(),_map.end(),[&keys](const std::pair<keytype,callback>& it){keys.push_back(it.first);});
	return keys;
}

template <typename base, typename keytype, typename ...args>
std::size_t SharedFactory<base,keytype,args...>::unregisterCallback(const keytype& key)
{
	return _map.erase(key);
}

} // end namespace nsx

#endif /* NSXTOOL_SharedFactory_H_ */
