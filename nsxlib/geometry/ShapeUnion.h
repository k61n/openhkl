/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin 
          Jonathan Fisher, Forschungszentrum Juelich GmbH
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

#ifndef NSXLIB_SHAPEUNION_H
#define NSXLIB_SHAPEUNION_H

#include <initializer_list>
#include <list>
#include <stdexcept>

#include <Eigen/Geometry>

namespace nsx {

template<typename T, unsigned int D>
class ShapeUnion {
public:
    using shape_t = IShape<T, D>;

    ShapeUnion() = default;
    ShapeUnion(const ShapeUnion& un);
    ShapeUnion(std::initializer_list<const shape_t&> lst);
    virtual ~ShapeUnion();

    void addShape(const shape_t& shape);


    virtual IShape<T,D>* clone() const;

    //! Double dispatching
    virtual bool collide(const IShape<T,D>& rhs) const;
    //! Interface for AABB collisions
    virtual bool collide(const AABB<T,D>& rhs) const;
    //! Interface for Ellipsoid collisions
    virtual bool collide(const Ellipsoid<T,D>& rhs) const;
    //! Interface for OBB collisions
    virtual bool collide(const OBB<T,D>& rhs) const;
    //! Interface for Sphere collisions
    virtual bool collide(const Sphere<T,D>& rhs) const;

private:
    std::list<shape_t*> _shapes;
};

// implementation

template<typename T, unsigned int D>
void ShapeUnion<T, D>::addShape(const shape_t& shape)
{
    _shapes.push_back(shape.clone());
}

template<typename T, unsigned int D>
ShapeUnion<T, D>::~ShapeUnion()
{
    for (auto&& shape: _shapes) {
        delete shape;
    }
}

template<typename T, unsigned int D>
ShapeUnion<T, D>::ShapeUnion(const ShapeUnion& other)
{
    for (auto&& shape: other._shapes) {
        addShape(*shape);
    }
}

template<typename T, unsigned int D>
ShapeUnion<T, D>::ShapeUnion(std::initializer_list<const shape_t&> shapes)
{
    for (auto&& shape: shapes) {
        addShape(shape);
    }
}

template<typename T, unsigned int D>
IShape<T,D>* ShapeUnion<T,D>::clone() const
{
#pragma warning "this is wrong!";
    // return new ShapeUnion<T, D>(*this);
    return nullptr;
}

template<typename T, unsigned int D>
bool ShapeUnion<T,D>::collide(const IShape<T,D>& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

template<typename T, unsigned int D>
bool ShapeUnion<T,D>::collide(const AABB<T,D>& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

template<typename T, unsigned int D>
bool ShapeUnion<T,D>::collide(const Ellipsoid<T,D>& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

template<typename T, unsigned int D>
bool ShapeUnion<T,D>::collide(const OBB<T,D>& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

template<typename T, unsigned int D>
bool ShapeUnion<T,D>::collide(const Sphere<T,D>& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

} // end namespace nsx

#endif // NSXLIB_SHAPEUNION_H
