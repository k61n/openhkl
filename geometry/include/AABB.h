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

#ifndef NSXTOOL_AABB_H_
#define NSXTOOL_AABB_H_

#include <initializer_list>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "IShape.h"
#include "Ellipsoid.h"
#include "OBB.h"
#include "Sphere.h"
#include "Types.h"

namespace SX
{

namespace Geometry
{


/*! \brief Axis-Aligned Bounding-Box in D dimension.
 *
 * AABB are used to bound objects in a simple-way since
 * their axes are aligned with the coordinates of the world.
 * Used for fast collision detection test, as well as a
 * way to iterate quickly over region of interest in data
 * from images or volumes.
 */
template<typename T, SX::Types::uint D>
class AABB : public IShape<T,D>
{
public:

	typedef Eigen::Matrix<T,D,D> matrix;
	typedef typename Eigen::Matrix<T,D,1> vector;
	typedef Eigen::Matrix<T,D+1,1> HomVector;

	// Get rid of IShape resolution for protected attributes of IShape
	using IShape<T,D>::_lowerBound;
	using IShape<T,D>::_upperBound;

	//! Constructs an unitialized AABB object
	AABB();
	//! Constructs a AABB object from another ABB object
	AABB(const AABB<T,D>& other);
	//! Constructs a AABB object from two Eigen vectors representing respectively its lower and upper bound
	AABB(const vector& lb, const vector& ub);
	//! Constructs a AABB object from two initializer lists representing respectively its lower and upper bound
	AABB(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);
	//! Destructor
	virtual ~AABB();
	//! Assignment operator
	AABB<T,D>& operator=(const AABB<T,D>& other);

	//! Constructs a pointer to a copy of the AABB
	IShape<T,D>* clone() const;

	//! Rotate the AABB.
	void rotate(const matrix& eigenvectors);
	//! Scale isotropically the AABB.
	void scale(T value);
	//! Scale anisotropically the AABB.
	void scale(const vector& scale);
	//! Translate the AABB.
	void translate(const vector& t);

	//! Check whether a point given as Homogeneous coordinate in the (D+1) dimension is inside the AABB.
	bool isInside(const HomVector& vector) const;
	//! Return true if the AABB intersects any kind of shape
	bool collide(const IShape<T,D>& other) const;
	//! Return true if the AABB intersects an ellipsoid.
	bool collide(const AABB<T,D>& other) const;
	//! Return true if the AABB intersects an ellipsoid.
	bool collide(const Ellipsoid<T,D>& other) const;
	//! Return true if the AABB intersects an OBB.
	bool collide(const OBB<T,D>& other) const;
	//! Return true if the AABB intersects a Sphere.
	bool collide(const Sphere<T,D>& other) const;

	//! Compute the intersection between the AABB and a given ray.
	//! Return true if an intersection was found, false otherwise.
	//! If the return value is true the intersection "times" will be stored
	//! in t1 and t2 in such a way that from + t1*dir and from + t2*dir are
	//! the two intersection points between the ray and this shape.
	bool rayIntersect(const vector& from, const vector& dir, double& t1, double& t2) const;

	// Macro to ensure that an OBB object can be dynamically allocated.
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

template<typename T,SX::Types::uint D> bool collideAABBAABB(const AABB<T,D>&, const AABB<T,D>&);
template<typename T,SX::Types::uint D> bool collideAABBEllipsoid(const AABB<T,D>&, const Ellipsoid<T,D>&);
template<typename T,SX::Types::uint D> bool collideAABBOBB(const AABB<T,D>&, const OBB<T,D>&);
template<typename T,SX::Types::uint D> bool collideAABBSphere(const AABB<T,D>&, const Sphere<T,D>&);

template<typename T, SX::Types::uint D>
AABB<T,D>::AABB() : IShape<T,D>()
{
}

template<typename T, SX::Types::uint D>
AABB<T,D>::AABB(const AABB<T,D>& other) : IShape<T,D>(other)
{
}

template<typename T, SX::Types::uint D>
AABB<T,D>::AABB(const vector& lb, const vector& ub) : IShape<T,D>(lb,ub)
{
}

template<typename T, SX::Types::uint D>
AABB<T,D>::AABB(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub) : IShape<T,D>(lb,ub)
{
}

template<typename T, SX::Types::uint D>
AABB<T,D>::~AABB()
{
}

template<typename T, SX::Types::uint D>
AABB<T,D>& AABB<T,D>::operator=(const AABB<T,D>& other)
{
  if (this != &other)
	  IShape<T,D>::operator=(other);
  return *this;
}

template<typename T,SX::Types::uint D>
IShape<T,D>* AABB<T,D>::clone() const
{
	return new AABB<T,D>(*this);
}

template<typename T, SX::Types::uint D>
void AABB<T,D>::rotate(const matrix& eigenvectors)
{
	_lowerBound=eigenvectors*_lowerBound;
	_upperBound=eigenvectors*_upperBound;
}

template<typename T, SX::Types::uint D>
void AABB<T,D>::scale(T value)
{
	IShape<T,D>::scaleAABB(value);
}

template<typename T,SX::Types::uint D>
void AABB<T,D>::scale(const vector& v)
{
	IShape<T,D>::scaleAABB(v);
}

template<typename T,SX::Types::uint D>
void AABB<T,D>::translate(const vector& t)
{
	IShape<T,D>::translateAABB(t);
}

template<typename T,SX::Types::uint D>
bool AABB<T,D>::isInside(const HomVector& vector) const
{
	return IShape<T,D>::isInsideAABB(vector);
}

template<typename T,SX::Types::uint D>
bool AABB<T,D>::collide(const IShape<T,D>& other) const
{
	if (this->intercept(other))
		return other.collide(*this);
	return false;
}

template<typename T,SX::Types::uint D>
bool AABB<T,D>::collide(const AABB<T,D>& other) const
{
	return collideAABBAABB<T,D>(*this,other);
}

template<typename T,SX::Types::uint D>
bool AABB<T,D>::collide(const Ellipsoid<T,D>& other) const
{
	return collideAABBEllipsoid<T,D>(*this,other);
}

template<typename T,SX::Types::uint D>
bool AABB<T,D>::collide(const OBB<T,D>& other) const
{
	return collideAABBOBB<T,D>(*this,other);
}

template<typename T,SX::Types::uint D>
bool AABB<T,D>::collide(const Sphere<T,D>& other) const
{
	return collideAABBSphere<T,D>(*this,other);
}

template<typename T, SX::Types::uint D>
bool AABB<T,D>::rayIntersect(const vector& from, const vector& dir, double& t1, double& t2) const
{
    // Adapted from
	// See http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
	// to get any values of t, negative or positive

	// Check first the case of ray // to one the box axis
    for (unsigned int i=0;i<D;++i)
	{
		if (std::abs(dir[i])<1.0e-10)
		{
			if (from[i] < _lowerBound[i] || from[i] > _upperBound[i])
				return false;
		}
	}

    // Keep track of t-values for every direction
    std::vector<double> tvalues;
    int nSlabs = 2*D;
    tvalues.reserve(nSlabs);

    // We test slabs in every direction
    for (unsigned int i=0; i<D; i++)
    {
		double invdir=1.0/dir[i];
		tvalues.push_back((_lowerBound[i] - from[i])*invdir);
		tvalues.push_back((_upperBound[i] - from[i])*invdir);
    }

    std::sort(tvalues.begin(),tvalues.end());

    t1 = tvalues[nSlabs/2 - 1];
    t2 = tvalues[nSlabs/2];

    double midt = (t1+t2)/2.0;

    vector halfvect = from + midt*dir;

    return IShape<T,D>::isInsideAABB(halfvect);
}

template<typename T,SX::Types::uint D>
bool collideAABBAABB(const AABB<T,D>& a, const AABB<T,D>& b)
{
	return a.collide(b);
}

template<typename T,SX::Types::uint D>
bool collideAABBEllipsoid(const AABB<T,D>& aabb, const Ellipsoid<T,D>& ell)
{
	return collideEllipsoidAABB(ell,aabb);
}

template<typename T,SX::Types::uint D>
bool collideAABBOBB(const AABB<T,D>& aabb, const OBB<T,D>& obb)
{
	return collideOBBAABB(obb,aabb);
}

template<typename T,SX::Types::uint D>
bool collideAABBSphere(const AABB<T,D>& aabb, const Sphere<T,D>& sphere)
{
	return collideSphereAABB(sphere,aabb);
}

} // namespace Geometry

} // namespace SX

#endif /*NSXTOOL_AABB_H_*/
