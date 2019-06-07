//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/geometry/BoxMask.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

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
