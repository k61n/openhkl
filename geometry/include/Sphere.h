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

#ifndef NSXTOOL_SPHERE_H_
#define NSXTOOL_SPHERE_H_

#include <cmath>
#include <initializer_list>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <unsupported/Eigen/MatrixFunctions>

#include "IShape.h"
#include "Ellipsoid.h"
#include "OBB.h"

namespace SX
{

namespace Geometry
{

template<typename T, uint D>
class Sphere : public IShape<T,D>
{

	// Some useful typedefs;
	typedef Eigen::Matrix<T,D,D> matrix;
	typedef Eigen::Matrix<T,D,1> vector;
	typedef Eigen::Matrix<T,D+1,1> HomVector;
	typedef Eigen::Matrix<T,D+1,D+1> HomMatrix;

	// Get rid of AABB resolution for protected attributes of AABB
	using AABB<T,D>::_lowerBound;
	using AABB<T,D>::_upperBound;

public:
	// Construct a N-dimensional sphere from its center and radius.
	Sphere(const vector& center, T radius);
	//; The destructor.
	~Sphere();
	//; Check whether two spheres collide.
	bool collide(const Sphere<T,D>& other) const;
	//; Check whether a sphere collides with an ellipsoid.
	bool collide(const Ellipsoid<T,D>&) const;
	//; Check whether a sphere collides with an OBB.
	bool collide(const OBB<T,D>&) const;
	//; Return the center of the sphere.
	const vector& getCenter() const;
	//; Return the radius of the sphere.
	T getRadius() const;
	//; Return the inverse of the Mapping matrix (\f$ S^{-1}.R^{-1}.T^{-1} \f$)
	HomMatrix getTRSInverseMatrix() const;
	//; Check whether a point given as Homogeneous coordinate in the (D+1) dimension is inside the sphere.
	bool isInside(const HomVector& vector) const;
	//; Scale the sphere.
	void scale(T value);
	//; Translate the sphere.
	void translate(const vector& t);


private:
	//; The center.
	vector _center;
	//; The scale value.
	T _radius;
	//; Update the AABB bound to the sphere.
	void updateAABB();

public:
	// Macro to ensure that Sphere object can be dynamically allocated.
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

template<typename T,uint D> bool collideSphereEllipsoid(const Sphere<T,D>&, const Ellipsoid<T,D>&);
template<typename T,uint D> bool collideSphereOBB(const Sphere<T,D>&, const OBB<T,D>&);
template<typename T,uint D> bool collideSphereSphere(const Sphere<T,D>&, const Sphere<T,D>&);

template<typename T,uint D>
Sphere<T,D>::Sphere(const vector& center, T radius)
: IShape<T,D>(), _center(center), _radius(radius)
{
	updateAABB();
}

template<typename T, uint D>
Sphere<T,D>::~Sphere()
{
}

template<typename T,uint D>
bool Sphere<T,D>::collide(const Sphere<T,D>& other) const
{
	return collideSphereSphere<T,D>(*this,other);
}

template<typename T,uint D>
bool Sphere<T,D>::collide(const Ellipsoid<T,D>& other) const
{
	return collideSphereEllipsoid<T,D>(*this,other);
}

template<typename T,uint D>
const typename Sphere<T,D>::vector& Sphere<T,D>::getCenter() const
{
	return _center;
}

template<typename T,uint D>
T Sphere<T,D>::getRadius() const
{
	return _radius;
}

template<typename T,uint D>
typename Sphere<T,D>::HomMatrix Sphere<T,D>::getTRSInverseMatrix() const
{
	Eigen::Matrix<T,D+1,D+1> mat=Eigen::Matrix<T,D+1,D+1>::Constant(0.0);
	mat(D,D)=1.0;
	for (unsigned int i=0;i<D+1;++i)
		mat(i,i)=1.0/_radius;
	mat.block(0,D,D,1)=-_center/_radius;

	return mat;
}

template<typename T, uint D>
bool Sphere<T,D>::isInside(const HomVector& point) const
{

	vector diff=point.segment(0,3)-_center;

	return (diff.squaredNorm()<(_radius*_radius));
}

template<typename T, uint D>
void Sphere<T,D>::scale(T value)
{
	_radius*=value;
	updateAABB();
}

template<typename T,uint D>
void Sphere<T,D>::translate(const vector& t)
{
	_center += t;
	updateAABB();
}

template<typename T, uint D>
void Sphere<T,D>::updateAABB()
{
	// Update the upper and lower bound of the AABB
	_lowerBound=_center.array()-_radius;
	_upperBound=_center.array()+_radius;

}

template<typename T,uint D>
bool collideSphereSphere(const Sphere<T,D>& a, const Sphere<T,D>& b)
{

	Eigen::Matrix<T,D,1> diff=b.getCenter()-a.getCenter();

	T sumRadii=a.getRadius()+b.getRadius();

    return (diff.squaredNorm()<(sumRadii*sumRadii));
}

template<typename T,uint D>
bool collideSphereEllipsoid(const Sphere<T,D>& s, const Ellipsoid<T,D>& eB)
{
	return collideEllipsoidSphere(eB,s);
}

template<typename T,uint D>
bool collideSphereOBB(const Sphere<T,D>& s, const Ellipsoid<T,D>& obb)
{
	Eigen::Matrix<T,D,1> scale=Eigen::Matrix<T,D,1>::Constant(s.getRadius());

	Eigen::Matrix<T,D,D> rot=Eigen::Matrix<T,D,D>::Identity();

	Ellipsoid<T,D> ell(s.getCenter(),scale,rot);

	return collideEllipsoidOBB(ell,obb);
}

} // namespace Geometry

} // namespace SX

#endif // NSXTOOL_SPHERE_H_

