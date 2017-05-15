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

namespace nsx {

template <typename keytype,typename storable>
class Store : public std::map<std::string,storable> {

public:
	Store();
	virtual ~Store();

	virtual storable get(const keytype& key)=0;
};

template <typename keytype,typename storable>
Store<keytype,storable>::Store() : std::map<keytype,storable>()
{
}

template <typename keytype,typename storable>
Store<keytype,storable>::~Store()
{
}

template <typename keytype,typename storable>
storable Store<keytype,storable>::get(const keytype& key)
{
}

} // end namespace nsx

#endif /* NSXTOOL_STORE_H_ */
