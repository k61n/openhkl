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

#ifndef NSXTOOL_STORE_H_
#define NSXTOOL_STORE_H_

#include <map>
#include <string>

namespace SX
{

namespace Kernel
{

template <typename storable>
class Store
{

public:
	Store();
	virtual ~Store();

	const storable* findObject(const std::string& key) const;
	const storable* findObject(const storable* object) const;
	const storable* retrieveObject(const std::string& key);
	bool hasObject(const std::string& key) const;
	bool hasObject(const storable* key) const;

private:
	std::map<std::string,storable*> _store;
};

template <typename storable>
Store<storable>::Store()
{
}

template <typename storable>
Store<storable>::~Store()
{
}

template <typename storable>
const storable* Store<storable>::findObject(const std::string& key) const
{
	auto it=_store.find(key);
	if (it != _store.end())
		return it->second;
	else
		return nullptr;
}

template <typename storable>
const storable* Store<storable>::findObject(const storable* object) const
{
	for (auto it=_store.begin();it!=_store.end();++it)
	{
		if (it->second == object)
			return object;
	}
	return nullptr;
}


template <typename storable>
const storable* Store<storable>::retrieveObject(const std::string& key)
{
	auto it=_store.find(key);
	if (it != _store.end())
		return it->second;
	else
	{
		storable* s=storable::build(key);
		if (s != nullptr)
			_store[key] = s;
		return s;
	}
}

template <typename storable>
bool Store<storable>::hasObject(const std::string& key) const
{
	auto it=_store.find(key);
	return (it != _store.end());
}

template <typename storable>
bool Store<storable>::hasObject(const storable* object) const
{
	for (auto it=_store.begin();it!=_store.end();++it)
	{
		if (it->second == object)
			return true;
	}
	return false;
}

} // end namespace Kernel

} // end namespace SX

#endif /* STORE_H_ */
