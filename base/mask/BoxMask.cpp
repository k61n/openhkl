//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/mask/BoxMask.cpp
//! @brief     Implements class BoxMask
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/mask/BoxMask.h"
#include "base/geometry/Ellipsoid.h"
#include "base/utils/Logger.h"
#include <iostream>

namespace nsx {

BoxMask::BoxMask(const AABB& aabb) : IMask(), _aabb(aabb)
{
    nsxlog(Level::Info, "BoxMask::Boxmask: Created new rectangular mask");
    nsxlog(Level::Info, "Lower bound:", _aabb.lower().transpose());
    nsxlog(Level::Info, "Upper bound:", _aabb.upper().transpose());
}

bool BoxMask::collide(const Ellipsoid& ellipsoid) const
{
    return _aabb.collide(ellipsoid);
}

IMask* BoxMask::clone() const
{
    return new BoxMask(*this);
}

} // namespace nsx
