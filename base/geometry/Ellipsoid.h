//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/Ellipsoid.h
//! @brief     Defines class Ellipsoid
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_GEOMETRY_ELLIPSOID_H
#define NSX_BASE_GEOMETRY_ELLIPSOID_H

#include "base/geometry/AABB.h"

#include <Eigen/Dense>

namespace nsx {

//! 3D Ellipsoid, stored as center-and-covariance matrix.

class Ellipsoid {
 public:
    Ellipsoid() = default;
    //! Construct directly from metric tensor
    Ellipsoid(const Eigen::Vector3d& center, const Eigen::Matrix3d& metric);
    //! Constructor for a sphere
    Ellipsoid(const Eigen::Vector3d& center, double radius);
    //! Construct a 3-dimensional ellipsoid from its center, radii, and principal axes ()
    Ellipsoid(
        const Eigen::Vector3d& center, const Eigen::Vector3d& radii, const Eigen::Matrix3d& axes);
    virtual ~Ellipsoid() = default;

    //! Returns true if the ellipsoid intersects an AABB.
    bool collide(const AABB& aabb) const;
    //! Returns true if the ellipsoid intersects an ellipsoid.
    bool collide(const Ellipsoid& other) const;
    //! Returns true if ellipsoid collides with segment from a to b
    bool collideSegment(const Eigen::Vector3d& a, const Eigen::Vector3d& b) const;
    //! Returns true if ellipsoid collides with face o (o+a) (o+a+b) (o+n) with normal n
    bool collideFace(
        const Eigen::Vector3d& o, const Eigen::Vector3d& a, const Eigen::Vector3d& b,
        const Eigen::Vector3d& n) const;
    //! Determine if given point is inside the ellipsoid
    bool isInside(const Eigen::Vector3d& point) const;
    //! Rotate the ellipsoid.
    void rotate(const Eigen::Matrix3d& U);
    //! Scale isotropically the ellipsoid.
    void scale(double value);
    //! Translate the ellipsoid
    void translate(const Eigen::Vector3d& t);
    //! Returns the volume of the ellipsoid
    double volume() const;
    //! Returns the homogenous matrix Q defining the ellipsoid
    Eigen::Matrix4d homogeneousMatrix() const;
    //! Returns the inverse of the homogenous matrix Q defining the ellipsoid
    Eigen::Matrix4d homogeneousMatrixInverse() const;
    //! Returns semiaxes of the ellipsoid
    Eigen::Vector3d radii() const;
    //! Returns the ellipsoid center
    const Eigen::Vector3d& center() const;
    //! Returns the ellipsoid metric (inverse covariance matrix)
    const Eigen::Matrix3d& metric() const;
    //! Returns the ellipsoid inverse metric (covariance matrix)
    const Eigen::Matrix3d& inverseMetric() const;
    //! Returns center of 2d ellipsoid formed by intersecting with plane with
    //! normal n and point p.
    Eigen::Vector3d intersectionCenter(const Eigen::Vector3d& n, const Eigen::Vector3d& p) const;
    //! Returns the axis aligned bounding box of the ellipsoid
    const AABB& aabb() const;
    //! Returns the value (x-center)*metric*(x-center)
    double r2(const Eigen::Vector3d x) const;

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

} // namespace nsx

#endif // NSX_BASE_GEOMETRY_ELLIPSOID_H
