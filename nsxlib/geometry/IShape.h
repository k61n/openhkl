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

#ifndef NSXLIB_ISHAPE_H
#define NSXLIB_ISHAPE_H

#include <iostream>
#include <initializer_list>
#include <stdexcept>

#include <Eigen/Geometry>

#include "../geometry/GeometryTypes.h"

namespace nsx {

/*! \brief Interface for Geometric Shapes in D dimensions.
 *
 * IShape is the interface for all Geometric Shapes in 2D and 3D.
 * Shape can receive homogeneous transformations such as scaling,
 * rotation and translation. Collisions with other shapes is
 * coded by double-dispatching.
 */

class IShape {

public:

    //! Construct an unitialized IShape
    IShape();
    //! Construct a IShape from another IShape
    IShape(const IShape& other);
    //! Construct a IShape from two Eigen vectors representing the lower and upper bound of its bounding box
    IShape(const Eigen::Vector3d& lower, const Eigen::Vector3d& upper);
    //! Construct a IShape from two initializer lists representing respectively its lower and upper bound
    IShape(const std::initializer_list<double>& lb, const std::initializer_list<double>& ub);
    // Destructor
    virtual ~IShape();

    IShape& operator=(const IShape& other);

    //! Return a pointer to a copy of the IShape object
    virtual IShape* clone() const=0;

    //! Double dispatching
    virtual bool collide(const IShape& rhs) const =0;
    //! Interface for AABB collisions
    virtual bool collide(const AABB& rhs) const =0;
    //! Interface for Ellipsoid collisions
    virtual bool collide(const Ellipsoid& rhs) const =0;
    //! Interface for OBB collisions
    virtual bool collide(const OBB& rhs) const =0;
    //! Interface for Sphere collisions
    virtual bool collide(const Sphere& rhs) const =0;

    //! Check whether the bounding box of the shape contains the bounding box of the another shape
    bool contains(const IShape& other) const;
    //! Check whether the bounding box of the shape intersects (e.g. touches or overlaps) the bounding box of another shape
    bool intercept(const IShape& other) const;

    //! Set the lower and upper bounds of the shape bounding box
    void setBounds(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub);
    //! Set the lower bound of the shape bounding box
    void setLower(const Eigen::Vector3d& lb);
    //! Set the upper bound of the shape bounding box
    void setUpper(const Eigen::Vector3d& lb);
    //! Get a constant reference to the lower bound of the bounding box of the shape
    const Eigen::Vector3d& getLower() const;
    //! Get a constant reference to the upper bound of the bounding box of the shape
    const Eigen::Vector3d& getUpper() const;
    //! Return the center of the bounding box of the shape
    Eigen::Vector3d getAABBCenter() const;
    //! Return the extends of the bounding box of the shape
    Eigen::Vector3d getAABBExtents() const;

    //! Returns the volume of the bounding box of the shape
    double AABBVolume() const;

    //! Check whether a given point is inside the AABB of the shape
    bool isInsideAABB(const std::initializer_list<double>& point) const;
    //! Check whether a given point is inside the AABB of the shape
    bool isInsideAABB(const Eigen::Vector3d& point) const;
    //! Check whether a given Homogeneous vector is inside the AABB of the shape
    bool isInsideAABB(const HomVector& point) const;

    //! True if the vector is inside the Shape
    //! @param vector : Homogeneous vector representing the point (x,y,z,1);
    virtual bool isInside(const HomVector& vector) const =0;

    virtual void rotate(const Eigen::Matrix3d& eigenvectors)=0;
    virtual void scale(double value) =0;
    virtual void translate(const Eigen::Vector3d& t) =0;

    void rotate(double angle,const Eigen::Vector3d& axis,Direction direction=Direction::CCW);
    //! Translate the bounding box
    void translateAABB(const Eigen::Vector3d&);
    //! Scale by a constant factor
    void scaleAABB(double);
    //! Scale the bounding box
    void scaleAABB(const Eigen::Vector3d&);

    //! Compute the intersection between the shape and a given ray.
    //! Return true if an intersection was found, false otherwise.
    //! If the return value is true the intersection "times" will be stored
    //! in t1 and t2 in such a way that from + t1*dir and from + t2*dir are
    //! the two intersection points between the ray and this shape.
    virtual bool rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const=0;

    std::ostream& printSelf(std::ostream& os) const;

#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif
protected:
    // The lower bound point
    Eigen::Vector3d _lowerBound;
    // The upper bound point
    Eigen::Vector3d _upperBound;
};

std::ostream& operator<<(std::ostream& os, const IShape& shape);

} // end namespace nsx

#endif // NSXLIB_ISHAPE_H
