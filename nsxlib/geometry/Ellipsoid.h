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

#ifndef NSXLIB_ELLIPSOID_H
#define NSXLIB_ELLIPSOID_H

#include <cmath>

#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>

#include "GeometryTypes.h"
#include "IShape.h"
//#include "AABB.h"
//#include "OBB.h"
//#include "Sphere.h"

namespace nsx {

class Ellipsoid : public IShape {

public:

    Ellipsoid();
    //! Copy constructor
    Ellipsoid(const Ellipsoid&);
    //! Construct a N-dimensional ellipsoid from its center, semi-axes, and eigenvectors ()
    Ellipsoid(const Eigen::Vector3d& center, const Eigen::Vector3d& eigenvalues, const Eigen::Matrix3d& eigenvectors);
    virtual ~Ellipsoid()=default; 
    //! Assignment
    Ellipsoid& operator=(const Ellipsoid&);
    //! Return a copy of this ellipsoid object
    IShape* clone() const;
    //! Return true if the ellipsoid intersects any kind of shape
    bool collide(const IShape& other) const;
    //! Return true if the ellipsoid intersects an aabb.
    bool collide(const AABB& other) const;
    //! Return true if the ellipsoid intersects an ellipsoid.
    bool collide(const Ellipsoid& other) const;
    //! Return true if the ellipsoid intersects an OBB.
    bool collide(const OBB& other) const;
    //! Return true if the ellipsoid intersects a Sphere.
    bool collide(const Sphere& other) const;
    //! Return the inverse of the Mapping matrix (\f$ S^{-1}.R^{-1}.T^{-1} \f$)
    const HomMatrix& getInverseTransformation() const;

    //! Check whether a point given as Homogeneous coordinate in the (D+1) dimension is Inside the Ellipsoid.
    bool isInside(const HomVector& vector) const;

    //! Rotate the ellipsoid.
    void rotate(const Eigen::Matrix3d& U);
    //! Scale isotropically the ellipsoid.
    void scale(double value);
    //! Translate the ellipsoid
    void translate(const Eigen::Vector3d& t);

    //! Compute the intersection between the sphere and a given ray.
    //! Return true if an intersection was found, false otherwise.
    //! If the return value is true the intersection "times" will be stored
    //! in t1 and t2 in such a way that from + t1*dir and from + t2*dir are
    //! the two intersection points between the ray and this shape.
    bool rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const;
    
    //! Return just the rotation and scaling matrix
    Eigen::Matrix3d getRSinv() const;

    //! Return the volume of the ellipsoid
    double getVolume() const;

    //! Return the homogenous matrix Q defining the ellipsoid
    Eigen::Matrix4d homogeneousMatrix() const;

    //! Return semiaxes of the ellipsoid
    Eigen::Vector3d eigenvalues() const;

#ifndef SWIG
    // Macro to ensure that Ellipsoid can be dynamically allocated.
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

private:
    //HomMatrix _TRSinv;
    // Method to update the closest fit AABB to the Ellipsoid
    void updateAABB();
    // EigenValues
    //Eigen::Vector3d _eigenVal;

    // new implementation
    Eigen::Matrix3d _metric;
    Eigen::Vector3d _center;
};

bool collideEllipsoidAABB(const Ellipsoid&, const AABB&);
//bool collideEllipsoidEllipsoid(const Ellipsoid&, const Ellipsoid&);
//bool collideEllipsoidOBB(const Ellipsoid&, const OBB&);
bool collideEllipsoidSphere(const Ellipsoid&, const Sphere&);

} // end namespace nsx

#endif // NSXLIB_ELLIPSOID_H
