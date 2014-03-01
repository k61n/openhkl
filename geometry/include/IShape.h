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
#include <Eigen/Geometry>
#include "AABB.h"

namespace SX
{

namespace Geometry
{

typedef unsigned int uint;

template<typename T, uint D> class Ellipsoid;
template<typename T, uint D> class OBB;
template<typename T, uint D> class Sphere;

enum Direction {CW,CCW};

template<typename T,uint D>
class IShape : public AABB<T,D>
{
	typedef Eigen::Matrix<T,D,D> matrix;
	typedef Eigen::Matrix<T,D,1> vector;
	typedef Eigen::Matrix<T,D+1,1> HomVector;
public:
	IShape();
	virtual ~IShape();

	virtual bool collide(const IShape<T,D>& rhs) const =0;
    virtual bool collide(const Ellipsoid<T,D>& rhs) const =0;
    virtual bool collide(const OBB<T,D>& rhs) const =0;
    virtual bool collide(const Sphere<T,D>& rhs) const =0;

	virtual bool isInside(const HomVector& vector) const =0;
	virtual void rotate(const matrix& eigenvectors) =0;
	void rotate(T angle,const vector& axis,Direction=CCW);
	virtual void scale(T value) =0;
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

template<typename T,uint D>
void IShape<T,D>::rotate(T angle,const vector& axis,Direction dir)
{
	if (dir==CW)
		angle*=-1;
	// Create the quaternion representing this rotation
	T hc=cos(0.5*angle);
	T hs=sin(0.5*angle);
	T norm=axis.norm();
	Eigen::Quaterniond temp(hc,axis(0)*hs/norm,axis(1)*hs/norm,axis(2)*hs/norm);
	return rotate(temp.toRotationMatrix());
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_ISHAPE_H_
