//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/mask/BoxMask.cpp
//! @brief     Implements class BoxMask
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/mask/BoxMask.h"
#include "base/geometry/Ellipsoid.h"
#include "base/utils/Logger.h"
#include <iostream>

namespace ohkl {

BoxMask::BoxMask(const AABB& aabb) : IMask(), _aabb(aabb)
{
    ohklLog(Level::Info, "BoxMask::Boxmask: Created new rectangular mask");
    ohklLog(Level::Info, "Lower bound: ", _aabb.lower().transpose());
    ohklLog(Level::Info, "Upper bound: ", _aabb.upper().transpose());
}

bool BoxMask::collide(const Ellipsoid& ellipsoid) const
{
    return _aabb.collide(ellipsoid);
}

IMask* BoxMask::clone() const
{
    return new BoxMask(*this);
}

} // namespace ohkl
