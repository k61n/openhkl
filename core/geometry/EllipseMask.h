//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/geometry/EllipseMask.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_GEOMETRY_ELLIPSEMASK_H
#define CORE_GEOMETRY_ELLIPSEMASK_H

#include "AABB.h"
#include "Ellipsoid.h"
#include "IMask.h"

namespace nsx {

//! Ellipsoidal detector mask
class EllipseMask : public IMask {

public:
    EllipseMask() = delete;

    EllipseMask(const EllipseMask& other) = default;

    EllipseMask(const AABB& aabb, bool two_dim = true);

    ~EllipseMask() = default;

    EllipseMask& operator=(const EllipseMask& other) = default;

    IMask* clone() const override;

    bool collide(const Ellipsoid& ellipsoid) const override;

private:
    Ellipsoid _ellipsoid;

    bool _2d;
};

} // end namespace nsx

#endif // CORE_GEOMETRY_ELLIPSEMASK_H
