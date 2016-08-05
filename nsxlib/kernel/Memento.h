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

#ifndef NSXTOOL_MEMENTO_H_
#define NSXTOOL_MEMENTO_H_

#include <stdexcept>
#include <unordered_map>

namespace SX
{

namespace Kernel
{

template <typename statetype>
class Memento
{
public:
	Memento(const statetype& s);
	const statetype& getState() const;

	virtual ~Memento();

private:
	Memento();

	statetype _state;
};

template <typename statetype>
Memento<statetype>::Memento()
{
}

template <typename statetype>
Memento<statetype>::Memento(const statetype& s) : _state(s)
{
}

template <typename statetype>
const statetype& Memento<statetype>::getState() const
{
	return _state;
}

template <typename statetype>
Memento<statetype>::~Memento()
{
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

template <typename key_type, typename mem_type>
class CareTaker
{
public:
	CareTaker();
	void add(const key_type& key,const mem_type& mem);
	const mem_type& get(const key_type& key) const;
	void remove(const key_type& key);

private:
	std::unordered_map<key_type,mem_type> _mementos;
};

template <typename key_type, typename mem_type>
CareTaker<key_type,mem_type>::CareTaker()
{
}

template <typename key_type, typename mem_type>
void CareTaker<key_type,mem_type>::add(const key_type& key,const mem_type& mem)
{
	auto it=_mementos.find(key);
	if (it!=_mementos.end())
		throw std::invalid_argument("Key already in use.");
	_mementos.insert({key,mem});
}

template <typename key_type, typename mem_type>
const mem_type& CareTaker<key_type,mem_type>::get(const key_type& key) const
{
	auto it=_mementos.find(key);
	if (it==_mementos.end())
		throw std::invalid_argument("Key not found.");
	return it->second;
}

template <typename key_type, typename mem_type>
void CareTaker<key_type,mem_type>::remove(const key_type& key)
{
	auto it=_mementos.find(key);
	if (it==_mementos.end())
		throw std::invalid_argument("Key not found.");
	_mementos.erase(key);
}

} // end namespace Kernel

} // end namespace SX

#endif /* NSXTOOL_MEMENTO_H_ */
