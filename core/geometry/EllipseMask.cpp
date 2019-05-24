#include "EllipseMask.h"

namespace nsx {

EllipseMask::EllipseMask(const AABB& aabb, bool two_dim) : IMask(), _ellipsoid(), _2d(two_dim)
{
    auto center = aabb.center();
    auto radii = 0.5 * (aabb.upper() - aabb.lower());
    auto axes = Eigen::Matrix3d::Identity();
    _ellipsoid = Ellipsoid(center, radii, axes);
}

bool EllipseMask::collide(const Ellipsoid& ellipsoid) const
{
    return _ellipsoid.collide(ellipsoid);
}

IMask* EllipseMask::clone() const
{
    return new EllipseMask(*this);
}

} // end namespace nsx
