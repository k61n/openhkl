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

#ifndef NSXTOOL_COMPOSITE_H_
#define NSXTOOL_COMPOSITE_H_

#include <set>

namespace SX
{

namespace Kernel
{

typedef unsigned int uint;

template <typename component>
class Composite : public component
{
public:

	typedef typename std::set<component*>::const_iterator component_const_iterator;
	typedef typename std::set<component*>::iterator component_iterator;

	//! Construct a Composite of component
	Composite();
	//! Construct a Composite of component from another one
	Composite(const Composite& other);

	//! Assignment operator
	Composite& operator=(const Composite& other);

	component* add(component*);
	void clear();
	uint getNComponents() const;
	void remove(component*);

	virtual ~Composite()=0;

protected:
	std::set<component*> _components;
};

template <typename component>
Composite<component>::Composite()
{
}

template <typename component>
Composite<component>::Composite(const Composite<component>& other)
{
	for (auto c : other._components)
		_components.insert(c->clone());
}

template <typename component>
Composite<component>& Composite<component>::operator=(const Composite<component>& other)
{
	if (this!=&other)
	{
		for (auto c : other._components)
			_components.insert(c->clone());
	}
	return *this;
}

template <typename component>
component* Composite<component>::add(component* comp)
{
	auto p=_components.insert(comp);
	return *(p.first);
}

template <typename component>
void Composite<component>::clear()
{
	_components.clear();
}

template <typename component>
uint Composite<component>::getNComponents() const
{
	return _components.size();
}

template <typename component>
void Composite<component>::remove(component* comp)
{
	auto it = _components.find(comp);
	if (it != _components.end())
		_components.erase(it);
}

template <typename component>
Composite<component>::~Composite()
{
}

} // end namespace Kernel

} // end namespace SX

#endif /* NSXTOOL_COMPOSITE_H_ */
