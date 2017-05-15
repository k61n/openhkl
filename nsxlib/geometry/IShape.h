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

#ifndef NSXTOOL_ISHAPE_H_
#define NSXTOOL_ISHAPE_H_

#include <iostream>
#include <initializer_list>
#include <stdexcept>

#include <Eigen/Geometry>

#include "../utils/Types.h"

namespace nsx {
namespace Geometry {

template<typename T, nsx::Types::uint D> class AABB;
template<typename T, nsx::Types::uint D> class Ellipsoid;
template<typename T, nsx::Types::uint D> class OBB;
template<typename T, nsx::Types::uint D> class Sphere;

enum Direction {CW,CCW};

/*! \brief Interface for Geometric Shapes in D dimensions.
 *
 * IShape is the interface for all Geometric Shapes in 2D and 3D.
 * Shape can receive homogeneous transformations such as scaling,
 * rotation and translation. Collisions with other shapes is
 * coded by double-dispatching.
 */
template<typename T,nsx::Types::uint D>
class IShape
{
public:
    using matrix = Eigen::Matrix<T,D,D>;
    using vector = Eigen::Matrix<T,D,1>;
    using HomVector = Eigen::Matrix<T,D+1,1>;

    //! Construct an unitialized IShape
    IShape();
    //! Construct a IShape from another IShape
    IShape(const IShape& other);
    //! Construct a IShape from two Eigen vectors representing the lower and upper bound of its bounding box
    IShape(const vector& lower, const vector& upper);
    //! Construct a IShape from two initializer lists representing respectively its lower and upper bound
    IShape(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub);
    // Destructor
    virtual ~IShape();

    IShape& operator=(const IShape& other);

    //! Return a pointer to a copy of the IShape object
    virtual IShape<T,D>* clone() const=0;

    //! Double dispatching
    virtual bool collide(const IShape& rhs) const =0;
    //! Interface for AABB collisions
    virtual bool collide(const AABB<T,D>& rhs) const =0;
    //! Interface for Ellipsoid collisions
    virtual bool collide(const Ellipsoid<T,D>& rhs) const =0;
    //! Interface for OBB collisions
    virtual bool collide(const OBB<T,D>& rhs) const =0;
    //! Interface for Sphere collisions
    virtual bool collide(const Sphere<T,D>& rhs) const =0;

    //! Check whether the bounding box of the shape contains the bounding box of the another shape
    bool contains(const IShape<T,D>& other) const;
    //! Check whether the bounding box of the shape intersects (e.g. touches or overlaps) the bounding box of another shape
    bool intercept(const IShape& other) const;

    //! Set the lower and upper bounds of the shape bounding box
    void setBounds(const vector& lb, const vector& ub);
    //! Set the lower bound of the shape bounding box
    void setLower(const vector& lb);
    //! Set the upper bound of the shape bounding box
    void setUpper(const vector& lb);
    //! Get a constant reference to the lower bound of the bounding box of the shape
    const vector& getLower() const;
    //! Get a reference to the lower bound of the bounding box of the shape
    vector& getLower();
    //! Get a constant reference to the upper bound of the bounding box of the shape
    const vector& getUpper() const;
    //! Get a reference to the upper bound of the bounding box of the shape
    vector& getUpper();
    //! Return the center of the bounding box of the shape
    vector getAABBCenter() const;
    //! Return the extends of the bounding box of the shape
    vector getAABBExtents() const;

    //! Returns the volume of the bounding box of the shape
    T AABBVolume() const;

    //! Check whether a given point is inside the AABB of the shape
    bool isInsideAABB(const std::initializer_list<T>& point) const;
    //! Check whether a given point is inside the AABB of the shape
    bool isInsideAABB(const vector& point) const;
    //! Check whether a given Homogeneous vector is inside the AABB of the shape
    bool isInsideAABB(const HomVector& point) const;

    //! True if the vector is inside the Shape
    //! @param vector : Homogeneous vector representing the point (x,y,z,1);
    virtual bool isInside(const HomVector& vector) const =0;

    virtual void rotate(const matrix& eigenvectors)=0;
    virtual void scale(T value) =0;
    virtual void translate(const vector& t) =0;

    void rotate(T angle,const vector& axis,Direction=CCW);
    //! Translate the bounding box
    void translateAABB(const vector&);
    //! Scale by a constant factor
    void scaleAABB(T);
    //! Scale the bounding box
    void scaleAABB(const vector&);

    //! Compute the intersection between the shape and a given ray.
    //! Return true if an intersection was found, false otherwise.
    //! If the return value is true the intersection "times" will be stored
    //! in t1 and t2 in such a way that from + t1*dir and from + t2*dir are
    //! the two intersection points between the ray and this shape.
    virtual bool rayIntersect(const vector& from, const vector& dir, double& t1, double& t2) const=0;

    std::ostream& printSelf(std::ostream& os);

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:
    // The lower bound point
    vector _lowerBound;
    // The upper bound point
    vector _upperBound;

};

template<typename T,nsx::Types::uint D>
IShape<T,D>::IShape()
{
}

template<typename T,nsx::Types::uint D>
IShape<T,D>::IShape(const IShape<T,D>& other)
{
    _lowerBound = other._lowerBound;
    _upperBound = other._upperBound;
}

template<typename T, nsx::Types::uint D>
IShape<T,D>::IShape(const vector& lb, const vector& ub) : _lowerBound(lb), _upperBound(ub)
{
    for (nsx::Types::uint i=0;i<D;++i)
    {
        if (_lowerBound(i)>_upperBound(i))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
    }
}

template<typename T, nsx::Types::uint D>
IShape<T,D>::IShape(const std::initializer_list<T>& lb, const std::initializer_list<T>& ub)
{
    auto it1 = lb.begin();
    auto it2 = ub.begin();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(;it1!=lb.end();it1++,it2++)
    {
        if ((*it1)>(*it2))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
        *(lbit++) = *it1;
        *(ubit++) = *it2;
    }
}

template<typename T,nsx::Types::uint D>
IShape<T,D>::~IShape()
{
}

template<typename T,nsx::Types::uint D>
IShape<T,D>& IShape<T,D>::operator=(const IShape<T,D>& other)
{
      if (this != &other)
      {
          _lowerBound = other._lowerBound;
          _upperBound = other._upperBound;
      }
      return *this;

}

template<typename T, nsx::Types::uint D>
bool IShape<T,D>::contains(const IShape<T,D>& other) const
{
    for (nsx::Types::uint i=0; i<D; ++i)
    {
        if (_lowerBound(i) >= other._lowerBound(i) || _upperBound(i) <= other._upperBound(i))
            return false;
    }
    return true;
}

template<typename T, nsx::Types::uint D>
bool IShape<T,D>::intercept(const IShape<T,D>& other) const
{
    for (nsx::Types::uint i = 0; i < D; ++i) {
        if (_upperBound(i) < other._lowerBound(i) || _lowerBound(i) > other._upperBound(i))
            return false;
    }
    return true;
}

template<typename T, nsx::Types::uint D>
void IShape<T,D>::setBounds(const vector& lb, const vector& ub)
{
    for (nsx::Types::uint i=0;i<D;++i) {
        if (lb(i)>ub(i))
            throw std::invalid_argument("IShape: upper limit must be > lower limit");
    }
    _lowerBound = lb;
    _upperBound = ub;
}

template<typename T, nsx::Types::uint D>
void IShape<T,D>::setLower(const vector& lb)
{
    for (nsx::Types::uint i=0;i<D;++i) {
        if (lb(i)>_upperBound(i))
            throw std::invalid_argument("IShape: upper limit must be > lower limit");
    }
    _lowerBound = lb;
}

template<typename T, nsx::Types::uint D>
void IShape<T,D>::setUpper(const vector& ub)
{
    for (nsx::Types::uint i=0;i<D;++i) {
        if (_lowerBound(i)>ub(i))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
    }
    _upperBound = ub;
}

template<typename T, nsx::Types::uint D>
const typename IShape<T,D>::vector& IShape<T,D>::getLower() const
{
    return _lowerBound;
}

template<typename T, nsx::Types::uint D>
typename IShape<T,D>::vector& IShape<T,D>::getLower()
{
    return _lowerBound;
}

template<typename T, nsx::Types::uint D>
const typename IShape<T,D>::vector& IShape<T,D>::getUpper() const
{
    return _upperBound;
}

template<typename T, nsx::Types::uint D>
typename IShape<T,D>::vector& IShape<T,D>::getUpper()
{
    return _upperBound;
}

template<typename T, nsx::Types::uint D>
T IShape<T,D>::AABBVolume() const
{
    return (_upperBound-_lowerBound).prod();
}

template<typename T, nsx::Types::uint D>
typename IShape<T,D>::vector IShape<T,D>::getAABBCenter() const
{
    vector center((_lowerBound + _upperBound)*0.5);
    return center;
}

template<typename T, nsx::Types::uint D>
typename IShape<T,D>::vector IShape<T,D>::getAABBExtents() const
{
    vector dim(_upperBound - _lowerBound);
    return dim;
}


template<typename T,nsx::Types::uint D>
bool IShape<T,D>::isInsideAABB(const std::initializer_list<T>& point) const
{

    if (point.size() != D)
        throw("AABB: invalid point size");

    auto it = point.begin();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(; it!=point.end(); it++,lbit++,ubit++) {
        if (*it < *lbit || *it > *ubit)
            return false;
    }

    return true;
}

template<typename T,nsx::Types::uint D>
bool IShape<T,D>::isInsideAABB(const vector& point) const
{

    auto it = point.data();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(unsigned int i=0; i<D; i++,lbit++,ubit++,it++) {
        if (*it < *lbit || *it > *ubit)
            return false;
    }

    return true;
}

template<typename T,nsx::Types::uint D>
bool IShape<T,D>::isInsideAABB(const HomVector& point) const
{

    auto it = point.data();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(unsigned int i=0; i<D; i++,lbit++,ubit++,it++) {
        if (*it < *lbit || *it > *ubit)
            return false;
    }

    return true;
}

template<typename T,nsx::Types::uint D>
std::ostream& operator<<(std::ostream& os, const IShape<T,D>& shape)
{
    return shape.printSelf(os);
}


template<typename T,nsx::Types::uint D>
std::ostream& IShape<T,D>::printSelf(std::ostream& os)
{
      os<<"AABB --> "<<"lower bound: "<<_lowerBound<<" , upper bound: "<<_upperBound;
      return os;
}

template<typename T, nsx::Types::uint D>
void IShape<T,D>::translateAABB(const vector& t)
{
    _lowerBound+=t;
    _upperBound+=t;
}

template<typename T, nsx::Types::uint D>
void IShape<T,D>::scaleAABB(const vector& s)
{
    vector center=IShape<T,D>::getAABBCenter();
    _lowerBound=center+(_lowerBound-center).cwiseProduct(s);
    _upperBound=center+(_upperBound-center).cwiseProduct(s);
}

template<typename T, nsx::Types::uint D>
void IShape<T,D>::scaleAABB(T s)
{
    vector center=IShape<T,D>::getAABBCenter();
    _lowerBound=center+(_lowerBound-center)*s;
    _upperBound=center+(_upperBound-center)*s;
}

template<typename T,nsx::Types::uint D>
void IShape<T,D>::rotate(T angle,const vector& axis,Direction dir)
{
    if (dir==CW)
        angle*=-1;
    // Create the quaternion representing this rotation
    T hc=cos(0.5*angle);
    T hs=sin(0.5*angle);
    T norm=axis.norm();
    Eigen::Quaterniond temp(hc,axis(0)*hs/norm,axis(1)*hs/norm,axis(2)*hs/norm);
    rotate(temp.toRotationMatrix());
}

} // namespace Geometry

} // end namespace nsx

#endif // NSXTOOL_ISHAPE_H_
