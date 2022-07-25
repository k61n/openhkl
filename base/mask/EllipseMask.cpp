//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/mask/EllipseMask.cpp
//! @brief     Implements class EllipseMask
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/mask/EllipseMask.h"

namespace ohkl {

EllipseMask::EllipseMask(const AABB& aabb, bool /*two_dim*/) : IMask(), _ellipsoid()
{
    auto center = aabb.center();
    auto radii = 0.5 * (aabb.upper() - aabb.lower());
    auto axes = Eigen::Matrix3d::Identity();
    _ellipsoid = Ellipsoid(center, radii, axes);
    ohklLog(Level::Info, "EllipseMask::EllipseMask: Created new elliptical mask");
    ohklLog(Level::Info, "Lower bound: ", aabb.lower().transpose());
    ohklLog(Level::Info, "Upper bound: ", aabb.upper().transpose());
}

bool EllipseMask::collide(const Ellipsoid& ellipsoid) const
{
    return _ellipsoid.collide(ellipsoid);
}

IMask* EllipseMask::clone() const
{
    return new EllipseMask(*this);
}

} // namespace ohkl
