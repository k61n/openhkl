//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/AABB.h
//! @brief     Defines class AABB
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_GEOMETRY_AABB_H
#define NSX_BASE_GEOMETRY_AABB_H

#include <Eigen/Dense>
#include <vector>

namespace ohkl {

class Ellipsoid; // cross dependence requires forward declaration

//! Axis-Aligned Bounding-Box in D dimension.

//! AABB are used to bound objects in a simple-way since
//! their axes are aligned with the coordinates of the world.
//! Used for fast collision detection test, as well as a way to
//! iterate quickly over region of interest in data from images or volumes.

class AABB {
 public:
    AABB() = default;
    //! Constructs a AABB object from two Eigen vectors representing respectively
    //! its lower and upper bound
    AABB(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub);
    virtual ~AABB() = default;

    //! Translates the AABB.
    void translate(const Eigen::Vector3d& t);

    //! Returns true if a point given as Homogeneous coordinate is inside the AABB.
    bool isInside(const Eigen::Vector3d& point) const;

    //! Returns true if the AABB intersects an ellipsoid.
    bool collide(const AABB& aabb) const;
    //! Returns true if the AABB intersects an ellipsoid.
    bool collide(const Ellipsoid& ellipsoid) const;

    //! Gets a constant reference to the lower bound of the bounding box of the shape
    const Eigen::Vector3d& lower() const;
    //! Sets the lower bound of the ellipsoid
    void setLower(const Eigen::Vector3d& lower);
    void setLower(double lowerx, double lowery, double lowerz);

    //! Gets a constant reference to the upper bound of the bounding box of the shape
    const Eigen::Vector3d& upper() const;
    //! Sets the upper bound of the ellipsoid
    void setUpper(const Eigen::Vector3d& upper);
    void setUpper(double upperx, double uppery, double upperz);

    //! Returns the center of the bounding box of the shape
    Eigen::Vector3d center() const;
    //! Returns the extends of the bounding box of the shape
    Eigen::Vector3d extents() const;

    //! Returns true if the bounding box of the shape contains the bounding box of
    //! the another shape
    bool contains(const AABB& other) const;

    std::ostream& printSelf(std::ostream& os) const;

#ifndef SWIG
    // Macro to ensure that an AABB object can be dynamically allocated.
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

 protected:
    //! The lower bound point
    Eigen::Vector3d _lowerBound;
    //! The upper bound point
    Eigen::Vector3d _upperBound;
};

std::ostream& operator<<(std::ostream& os, const AABB& aabb);

} // namespace ohkl

#endif // NSX_BASE_GEOMETRY_AABB_H
