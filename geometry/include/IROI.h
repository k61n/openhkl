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

#ifndef NSXTOOL_IROI_H_
#define NSXTOOL_IROI_H_

#include <memory>

#include "ShapeCollection.h"
#include "IData.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

template<typename TYPE, typename DATATYPE, uint DIM>
class IROI : public ShapeCollection<TYPE,DIM>
{
public:

	typedef SX::Data::IData<DATATYPE,DIM> idata;

	IROI();

	virtual ~IROI();

	void attach_data(std::shared_ptr<idata> data);

private:

	std::shared_ptr<idata> _data;

};

template<typename TYPE, typename DATATYPE, uint DIM>
IROI<TYPE,DATATYPE,DIM>::IROI() : ShapeCollection<TYPE,DIM>()
{

}

template<typename TYPE, typename DATATYPE, uint DIM>
IROI<TYPE,DATATYPE,DIM>::~IROI()
{
	_data.reset();
}

template<typename TYPE, typename DATATYPE, uint DIM>
void IROI<TYPE,DATATYPE,DIM>::attach_data(std::shared_ptr<idata> data)
{
	_data = data;
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_IROI_H_
