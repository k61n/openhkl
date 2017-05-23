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

#ifndef NSXLIB_SPHERE_H
#define NSXLIB_SPHERE_H

#include <cmath>
#include <initializer_list>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <unsupported/Eigen/MatrixFunctions>

#include "IShape.h"
#include "AABB.h"
#include "Ellipsoid.h"
#include "OBB.h"

namespace nsx {

class Sphere : public IShape {

public:
    //! The copy constructor
    Sphere(const Sphere& other);
    //! Construct a N-dimensional sphere from its center and radius.
    Sphere(const Eigen::Vector3d& center, double radius);
    //! Return a copy of this Sphere
    IShape* clone() const;
    //! The destructor.
    virtual ~Sphere()=default;
    //! The assignement operator
    Sphere& operator=(const Sphere& other);
    //! Return true if the sphere intersects any kind of shape.
    bool collide(const IShape& other) const;
    //! Check whether a sphere collides with a AABB.
    bool collide(const AABB&) const;
    //! Check whether a sphere collides with an ellipsoid.
    bool collide(const Ellipsoid&) const;
    //! Check whether a sphere collides with an OBB.
    bool collide(const OBB&) const;
    //! Check whether two spheres collide.
    bool collide(const Sphere& other) const;
    //! Return the center of the sphere.
    const Eigen::Vector3d& getCenter() const;
    //! Return the radius of the sphere.
    double getRadius() const;
    //! Return the inverse of the Mapping matrix (\f$ S^{-1}.R^{-1}.T^{-1} \f$)
    HomMatrix getInverseTransformation() const;
    //! Check whether a point given as Homogeneous coordinate in the (D+1) dimension is inside the sphere.
    bool isInside(const HomVector& vector) const;
    //! Rotate the sphere
    void rotate(const Eigen::Matrix3d& eigenvectors);
    //! Scale the sphere.
    void scale(double value);
    //! Translate the sphere.
    void translate(const Eigen::Vector3d& t);

    //! Compute the intersection between the sphere and a given ray.
    //! Return true if an intersection was found, false otherwise.
    //! If the return value is true the intersection "times" will be stored
    //! in t1 and t2 in such a way that from + t1*dir and from + t2*dir are
    //! the two intersection points between the ray and this shape.
    bool rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const;

#ifndef SWIG
    // Macro to ensure that Sphere object can be dynamically allocated.
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

private:
    //! The center.
    Eigen::Vector3d _center;
    //! The scale value.
    double _radius;
    //! Update the AABB bound to the sphere.
    void updateAABB();

};

bool collideSphereAABB(const Sphere&, const AABB&);
bool collideSphereEllipsoid(const Sphere&, const Ellipsoid&);
bool collideSphereOBB(const Sphere&, const OBB&);
bool collideSphereSphere(const Sphere&, const Sphere&);

} // end namespace nsx

#endif // NSXLIB_SPHERE_H

