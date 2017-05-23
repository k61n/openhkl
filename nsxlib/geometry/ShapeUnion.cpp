#include <initializer_list>
#include <list>
#include <stdexcept>

#include <Eigen/Geometry>

namespace nsx {

void ShapeUnion::addShape(const IShape& shape)
{
    _shapes.push_back(shape.clone());
}

ShapeUnion::~ShapeUnion()
{
    for (auto&& shape: _shapes) {
        delete shape;
    }
}

ShapeUnion::ShapeUnion(const ShapeUnion& other)
{
    for (auto&& shape: other._shapes) {
        addShape(*shape);
    }
}

ShapeUnion::ShapeUnion(std::initializer_list<const IShape&> shapes)
{
    for (auto&& shape: shapes) {
        addShape(shape);
    }
}

IShape* ShapeUnion::clone() const
{
#pragma warning "this is wrong!";
    // return new ShapeUnion<T, D>(*this);
    return nullptr;
}

bool ShapeUnion::collide(const IShape& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

bool ShapeUnion::collide(const AABB& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

bool ShapeUnion::collide(const Ellipsoid& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

bool ShapeUnion::collide(const OBB& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

bool ShapeUnion::collide(const Sphere& rhs) const
{
    for (auto&& shape: _shapes) {
        if (shape->collide(rhs)) {
            return true;
        }
    }
    return false;
}

} // end namespace nsx

