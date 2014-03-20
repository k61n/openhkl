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

namespace SX
{

namespace Kernel
{

template <typename statetype>
class Memento
{
public:
	Memento(const statetype& s);
	const statetype& getState();

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
const statetype& Memento<statetype>::getState()
{
	return _state;
}

template <typename statetype>
Memento<statetype>::~Memento()
{
}

} // end namespace Kernel

} // end namespace SX

#endif /* MEMENTO_H_ */
