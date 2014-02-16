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

#ifndef NSXTOOL_IPSHAPE_H_
#define NSXTOOL_IPSHAPE_H_

#include <initializer_list>

#include "AABB.h"

namespace SX
{

namespace Geometry
{

template<typename T, unsigned int D> class NDBox;
template<typename T, unsigned int D> class ShapeCollection;

typedef unsigned int uint;

template<typename T,uint D>
class IShape : public AABB<T,D>
{
	typedef Eigen::Matrix<T,D+1,1> HomVector;
public:
	IShape();
	virtual ~IShape();
	virtual bool isInside(const HomVector& vector) const =0;
    //virtual bool collide(const IShape<T,D>& rhs) const =0;
    //virtual bool collide(const NDBox<T,D>& rhs) const =0;
    //virtual bool collide(const ShapeCollection<T,D>& rhs) =0;
};

template<typename T,uint D>
IShape<T,D>::IShape() : AABB<T,D>()
{
}

template<typename T,uint D>
IShape<T,D>::~IShape()
{
}

template<typename T, unsigned int D>
bool collisionBoxBox(const NDBox<T,D>& b1, const NDBox<T,D>& b2)
{
	return b1.collide(b2);
}

template<typename T, unsigned int D>
bool collisionBoxShapeCollection(const NDBox<T,D>& b1, const ShapeCollection<T,D>& b2)
{
	return b1.collide(b2);
}


} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_IPSHAPE_H_
