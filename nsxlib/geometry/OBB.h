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

#ifndef NSXLIB_OBB_H
#define NSXLIB_OBB_H

#include <cmath>
#include <initializer_list>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>
#include <unsupported/Eigen/MatrixFunctions>

#include "AABB.h"
#include "Ellipsoid.h"
#include "IShape.h"
#include "Sphere.h"

namespace nsx {

class OBB : public IShape {

public:

    //! Default constructor
    OBB();
    //! Copy constructor
    OBB(const OBB&);
    //! Construct a N-dimensional box from its center, semi-axes, and eigenvectors ()
    OBB(const Eigen::Vector3d& center, const Eigen::Vector3d& eigenvalues, const Eigen::Matrix3d& eigenvectors);
    //! Construct a OBB from a AABB
    OBB(const AABB& aabb);
    //! Assignment
    OBB& operator=(const OBB& other);
    //! Return a copy of this OBB
    IShape* clone() const;
    //! The destructor.
    virtual ~OBB()=default;
    //! Return true if the OBB intersects any kind of shape.
    bool collide(const IShape& other) const;
    //! Returns true if the OBB collides with a AABB.
    bool collide(const AABB&) const;
    //! Returns true if the OBB collides with an Ellipsoid.
    bool collide(const Ellipsoid&) const;
    //! Returns true if the OBB collides with an OBB.
    bool collide(const OBB& other) const;
    //! Returns true if the OBB collides with a Sphere.
    bool collide(const Sphere&) const;
    //! Return the extents of the OBB
    const Eigen::Vector3d& getExtents() const;
    //! Return the inverse of the Mapping matrix (\f$ S^{-1}.R^{-1}.T^{-1} \f$)
    const HomMatrix& getInverseTransformation() const;
    //! Check whether a point given as Homogeneous coordinate in the (D+1) dimension is inside the OBB.
    bool isInside(const HomVector& vector) const;
    //! Rotate the OBB.
    void rotate(const Eigen::Matrix3d& eigenvectors);
    //! Scale isotropically the OBB.
    void scale(double value);
    //! Scale anisotropically the OBB.
    void scale(const Eigen::Vector3d& scale);
    //! Translate the OBB.
    void translate(const Eigen::Vector3d& t);

    //! Compute the intersection between the OBB and a given ray.
    //! Return true if an intersection was found, false otherwise.
    //! If the return value is true the intersection "times" will be stored
    //! in t1 and t2 in such a way that from + t1*dir and from + t2*dir are
    //! the two intersection points between the ray and this shape.
    bool rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const;

private:
    //! The inverse of the homogeneous transformation matrix.
    HomMatrix _TRSinv;
    //! The scale value.
    Eigen::Vector3d _eigenVal;
    //! Update the closest fit AABB to the OBB.
    void updateAABB();

public:

#ifndef SWIG
    // Macro to ensure that an OBB object can be dynamically allocated.
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

};

// Collision detection in the 3D case.
bool collideOBBAABB(const OBB&, const AABB&);
bool collideOBBEllipsoid(const OBB&, const Ellipsoid&);
bool collideOBBOBB(const OBB&, const OBB&);
bool collideOBBSphere(const OBB&, const Sphere&);

} // end namespace nsx

#endif // NSXLIB_OBB_H

