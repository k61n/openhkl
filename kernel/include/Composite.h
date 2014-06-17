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

#include <vector>

namespace SX
{

namespace Kernel
{

typedef unsigned int uint;

template <typename component>
class Composite : public component
{
public:

	Composite();

	virtual void add(component*);
	void clear();
	uint nComponents() const;
	void remove(component*);

	virtual ~Composite()=0;

protected:
	std::vector<component*> _components;
};

template <typename component>
Composite<component>::Composite()
{
}

template <typename component>
void Composite<component>::add(component* comp)
{

	auto it = std::find(_components.begin(),_components.end(),comp);
	if (it == _components.end())
		_components.push_back(comp);
}

template <typename component>
void Composite<component>::clear()
{
	_components.clear();
}

template <typename component>
uint Composite<component>::nComponents() const
{
	return _components.size();
}

template <typename component>
void Composite<component>::remove(component* comp)
{
	auto it = std::find(_components.begin(),_components.end(),comp);
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
