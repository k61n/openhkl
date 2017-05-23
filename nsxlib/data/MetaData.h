/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
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

#ifndef NSXLIB_METADATA_H
#define NSXLIB_METADATA_H

#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include "../data/DataTypes.h"

namespace nsx {

/*! \brief Class to store MetaData of a DataSet stored by a map of key/value pair
 *
 *  MetaData class allow to store metadata associated with a data file  into a map indexed by the string
 * of the entry. Any parameter type can be stored, using a template argument. Internally, MetaData
 * stores the value using boost::any.
 * A specific key can be retrieved using the templated type if known.
 * From C++0x onwards, a parameter can be retrieved even if its type is unknown by the user
 * using auto.
 */
class MetaData
{
public:
	//! Constructor
	MetaData();
	//! Destructor
	~MetaData();
	//! Copy
	MetaData(const MetaData&);
	//! Assignment
	MetaData& operator=(const MetaData&);
	//! Add a pair of key and value.
	template <class _type> void add(const std::string& key, const _type& value);
	//! Get the value associated with the key. User must know the return type, otherwise use method that return boost_any
	template <class _type>  _type getKey(const std::string& key) const;
	//! Get the value associated with the key. User must know the return type, otherwise use method that return boost_any
	template <class _type>  _type getKey(const char* key) const;
	//! Return the value as boost::any. Explicit casting with boost::any_cast must be performed.
	//@ return : value corresponding to key
	boost::any getKey(const std::string& key) const;
	//! Is this key in the metadata
	bool isKey(const std::string& key) const;
	//!Is this key in the metadata
	bool isKey(const char* key) const;
	//! Number of elements in the map
	//@ return : Number of elements in the map
	//std::size_t size() const;
	//! Get all the keys available.
	const MetaDataKeySet& getAllKeys() const;
	//! Get the full map of parameters
	const MetaDataMap& getMap() const;
private:
	//! Contains the map of all key/value pairs.
	MetaDataMap _map;
	//! Contains all available keys so far.
	static MetaDataKeySet  _metakeys;

};

template <typename _type>
void MetaData::add(const std::string& key,const _type& value)
{
	// First, make sure the key is already in the keyset
	// Since this is a set, no duplicate will be found. No need to search explicitely.
	std::pair<MetaDataKeySet::iterator, bool> it=_metakeys.insert(key);

	//  If all OK, then add the key to the map
	const char* ptr=(*it.first).c_str();
	_map.insert(std::pair<const char*,_type>(ptr,value));
}

template <typename _type>
 _type MetaData::getKey(const std::string& key) const
{
	// Search if this key is in the set.
	auto it=_metakeys.find(key);
	if (it==_metakeys.end())
		throw std::runtime_error("Could not find key :"+key+" in MetaData");
	// Then search in the map
	const char* ptr=(*it).c_str();
	auto it2=_map.find(ptr);
	if (it2==_map.end())
		throw std::runtime_error("Could not find key :"+key+" in MetaData");
	return boost::any_cast<_type>((*it2).second);
}

template <typename _type>
 _type MetaData::getKey(const char* key) const
{
	return getKey<_type>(std::string(key));
}

} // end namespace nsx

#endif // NSXLIB_METADATA_H
