#include "BoxMask.h"
#include "Ellipsoid.h"

namespace nsx {

BoxMask::BoxMask(const AABB& aabb) : IMask(), _aabb(aabb) {}

bool BoxMask::collide(const Ellipsoid& ellipsoid) const
{
    return _aabb.collide(ellipsoid);
}

IMask* BoxMask::clone() const
{
    return new BoxMask(*this);
}

} // end namespace nsx
