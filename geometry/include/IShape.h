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

#ifndef NSXTOOL_ISHAPE_H_
#define NSXTOOL_ISHAPE_H_

#include <initializer_list>

#include "AABB.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

template<typename T, uint D> class Ellipsoid;
template<typename T, uint D> class OBB;
template<typename T, uint D> class Sphere;

template<typename T,uint D>
class IShape : public AABB<T,D>
{
	typedef Eigen::Matrix<T,D,1> vector;
	typedef Eigen::Matrix<T,D+1,1> HomVector;
public:
	IShape();
	virtual ~IShape();
	virtual bool isInside(const HomVector& vector) const =0;
    virtual bool collide(const IShape<T,D>& rhs) const =0;
    virtual bool collide(const Ellipsoid<T,D>& rhs) const =0;
    virtual bool collide(const OBB<T,D>& rhs) const =0;
    virtual bool collide(const Sphere<T,D>& rhs) const =0;

	virtual void translate(const vector& t) =0;
};

template<typename T,uint D>
IShape<T,D>::IShape() : AABB<T,D>()
{
}

template<typename T,uint D>
IShape<T,D>::~IShape()
{
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_ISHAPE_H_
