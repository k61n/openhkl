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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
 *
 */

#pragma once

#include <initializer_list>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "GeometryTypes.h"

namespace nsx {

/*! \brief Axis-Aligned Bounding-Box in D dimension.
 *
 * AABB are used to bound objects in a simple-way since
 * their axes are aligned with the coordinates of the world.
 * Used for fast collision detection test, as well as a
 * way to iterate quickly over region of interest in data
 * from images or volumes.
 */
class AABB {

public:
    //! Constructs an unitialized AABB object
    AABB();
    //! Constructs a AABB object from another ABB object
    AABB(const AABB& other);
    //! Constructs a AABB object from two Eigen vectors representing respectively
    //! its lower and upper bound
    AABB(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub);
    //! Destructor
    virtual ~AABB() = default;

    //! Assignment operator
    AABB& operator=(const AABB& other);

    //! Translate the AABB.
    void translate(const Eigen::Vector3d& t);

    //! Check whether a point given as Homogeneous coordinate is inside the AABB.
    bool isInside(const Eigen::Vector3d& vector) const;

    //! Return true if the AABB intersects an ellipsoid.
    bool collide(const AABB& aabb) const;
    //! Return true if the AABB intersects an ellipsoid.
    bool collide(const Ellipsoid& ellipsoid) const;

    //! Get a constant reference to the lower bound of the bounding box of the
    //! shape
    const Eigen::Vector3d& lower() const;
    //! Set the lower bound of the ellipsoid
    void setLower(const Eigen::Vector3d& lower);

    //! Get a constant reference to the upper bound of the bounding box of the
    //! shape
    const Eigen::Vector3d& upper() const;
    //! Set the upper bound of the ellipsoid
    void setUpper(const Eigen::Vector3d& upper);

    //! Return the center of the bounding box of the shape
    Eigen::Vector3d center() const;
    //! Return the extends of the bounding box of the shape
    Eigen::Vector3d extents() const;

    //! Check whether the bounding box of the shape contains the bounding box of
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

} // end namespace nsx
