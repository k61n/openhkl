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

#ifndef NSXTOOL_PEAK_H_
#define NSXTOOL_PEAK_H_

#include "IROI.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

template<typename TYPE, typename DATATYPE, uint DIM>
class Peak : public IROI<TYPE,DATATYPE,DIM>
{
public:

	Peak();

	//! add the background region to the peak
	void add_background(const IPShape<TYPE,DIM>& bg);

	//! add the signal region to the peak
	void add_signal(const IPShape<TYPE,DIM>& peak);

	//! return whether or not a given point is inside the peak region (peak+bg)
	virtual bool is_inside(const std::initializer_list<TYPE>& point) const;

	//! return whether or not a given point is inside the signal region
	bool is_inside_signal(const std::initializer_list<TYPE>& point) const;

	//! return whether or not a given point is inside the background region
	bool is_inside_background(const std::initializer_list<TYPE>& point) const;

	virtual ~Peak();

};

template<typename TYPE, typename DATATYPE, uint DIM>
Peak<TYPE,DATATYPE,DIM>::Peak() : IROI<TYPE,DATATYPE,DIM>()
{
}

template<typename TYPE, typename DATATYPE, uint DIM>
Peak<TYPE,DATATYPE,DIM>::~Peak()
{
}

template<typename TYPE, typename DATATYPE, uint DIM>
void Peak<TYPE,DATATYPE,DIM>::add_signal(const IPShape<TYPE,DIM>& peak)
{
	if (!(this->_shapes.empty()))
		throw("Peak:the signal must be the first region to be inserted");

	this->add_shape(peak);
}

template<typename TYPE, typename DATATYPE, uint DIM>
void Peak<TYPE,DATATYPE,DIM>::add_background(const IPShape<TYPE,DIM>& bg)
{
	if (this->_shapes.size()!=1)
		throw("Peak:the background must be inserted after the signal");

	this->add_shape(bg);
}

template<typename TYPE, typename DATATYPE, uint DIM>
bool Peak<TYPE,DATATYPE,DIM>::is_inside(const std::initializer_list<TYPE>& point) const
{
	if (this->_shapes.size()!=2)
		throw("Peak:the peak is not fully defined");

	return (this->_shapes[0]->is_inside(point) || this->_shapes[1]->is_inside(point));

}

template<typename TYPE, typename DATATYPE, uint DIM>
bool Peak<TYPE,DATATYPE,DIM>::is_inside_signal(const std::initializer_list<TYPE>& point) const
{
	if (this->_shapes.size()!=2)
		throw("Peak:the peak is not fully defined");

	return this->_shapes[0]->is_inside(point);

}

template<typename TYPE, typename DATATYPE, uint DIM>
bool Peak<TYPE,DATATYPE,DIM>::is_inside_background(const std::initializer_list<TYPE>& point) const
{
	if (this->_shapes.size()!=2)
		throw("Peak:the peak is not fully defined");

	return this->_shapes[1]->is_inside(point);

}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_PEAK_H_
