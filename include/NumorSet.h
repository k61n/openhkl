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

#ifndef  SX_NumorSet_H_
#define SX_NumorSet_H_
#include "Numor.h"
#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

// A few typedefs.
typedef boost::shared_ptr<SX::Numor> sptrNumor;
typedef std::map<int,sptrNumor> numMap;
typedef numMap::iterator numMapit;
typedef numMap::const_iterator numMapconstit;

namespace SX
{
//! Class to store and handle a set of Numor files.
class NumorSet
{
public:
	//! Construct a numor set with a given name
	NumorSet(const char * name);
	virtual ~NumorSet();
	//! Read a numor from a file and add it to the map, if not already existing.
	void addNumorFromFile(const std::string& filename);
	//! Add numor already existing to the collection
	void addNumor(const sptrNumor& clone);
	//! Delete numor will onlly delete this shared_ptr instance since data is shared.
	void deleteNumor(int numor);
	//! Is this numor present in the set.
	bool isNumor(int numor) const;
	//! Get the number of numors in this set
	std::size_t  getnElements() const;
	//! Get the numor, return null if not present
	sptrNumor getNumor(int numor);
	//! Get all numors as vector
	std::vector<sptrNumor> getNumors();
	// ! Return the numors key (int) of the numors in the set.
	std::vector<int> getNumorKeys() const;
	//! Return the metadata values of a given key as a vector. This can be used to
	//! retrieve temperature etc... as a function of Numor number or time..
	template <typename T> std::vector<T> getMValues(const std::string& key);
	//! Filtering
private:
	std::string _name;
	numMap _numors;
};

template<typename T>
std::vector<T> NumorSet::getMValues(const std::string& key)
{
	std::vector<T> result;
	result.reserve(_numors.size());
	numMapit it=_numors.begin();
	for (;it!=_numors.end();++it)
	{
		result.push_back(it->second->getMetaData().getKey<T>(key));
	}
	return result;
}


} /* namespace SX */
#endif /* SX_NumorSet_H_ */
