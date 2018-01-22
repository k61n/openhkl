/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
    2016- Laurent C. Chapon Eric Pellegrini, Jonathan Fisher
    Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

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

#pragma once

#include <cmath>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/Geometry>

#include "AABB.h"
#include "GeometryTypes.h"

namespace nsx {

struct EllipsoidParameters {
    Eigen::Vector3d center;
    Eigen::Vector3d radii;
    Eigen::Matrix3d axes;
};

class Ellipsoid {

public:

    Ellipsoid();
    //! Copy constructor
    Ellipsoid(const Ellipsoid& other);
    //! Construct directly from metric tensor
    Ellipsoid(const Eigen::Vector3d& center, const Eigen::Matrix3d& metric);
    //! Constructor for a sphere
    Ellipsoid(const Eigen::Vector3d& center, double radius);
    //! Construct a 3-dimensional ellipsoid from its center, radii, and principal axes ()
    Ellipsoid(const Eigen::Vector3d& center, const Eigen::Vector3d& radii, const Eigen::Matrix3d& axes);
    //! Destructor
    virtual ~Ellipsoid()=default; 
    //! Assignment
    Ellipsoid& operator=(const Ellipsoid& other);
    //! Return true if the ellipsoid intersects an aabb.
    bool collide(const AABB& other) const;
    //! Return true if the ellipsoid intersects an ellipsoid.
    bool collide(const Ellipsoid& other) const;    
    //! Return true if ellipsoid collides with segment from a to b
    bool collideSegment(const Eigen::Vector3d& a, const Eigen::Vector3d& b) const;
    //! Return true if ellipsoid collides with face o (o+a) (o+a+b) (o+n) with normal n
    bool collideFace(const Eigen::Vector3d& o, const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& n) const;

    bool isInside(const Eigen::Vector3d& point) const;

    //! Rotate the ellipsoid.
    void rotate(const Eigen::Matrix3d& U);
    //! Scale isotropically the ellipsoid.
    void scale(double value);
    //! Translate the ellipsoid
    void translate(const Eigen::Vector3d& t);
    
    //! Return the volume of the ellipsoid
    double volume() const;

    //! Return the homogenous matrix Q defining the ellipsoid
    Eigen::Matrix4d homogeneousMatrix() const;

    //! Return the inverse of the homogenous matrix Q defining the ellipsoid
    Eigen::Matrix4d homogeneousMatrixInverse() const;

    //! Return semiaxes of the ellipsoid
    Eigen::Vector3d radii() const;

    EllipsoidParameters parameters() const;

    const Eigen::Vector3d& center() const;

    const Eigen::Matrix3d& metric() const;
    const Eigen::Matrix3d& inverseMetric() const;

    const AABB& aabb() const;

#ifndef SWIG
    // Macro to ensure that Ellipsoid can be dynamically allocated.
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

private:
    void updateAABB();
    Eigen::Vector3d _center;
    Eigen::Matrix3d _metric;
    Eigen::Matrix3d _inverseMetric;
    AABB _aabb;
};

} // end namespace nsx
