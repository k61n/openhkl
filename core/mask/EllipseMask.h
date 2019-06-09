//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/mask/EllipseMask.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_MASK_ELLIPSEMASK_H
#define CORE_MASK_ELLIPSEMASK_H

#include "core/geometry/AABB.h"
#include "core/geometry/Ellipsoid.h"
#include "core/mask/IMask.h"

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

} // namespace nsx

#endif // CORE_MASK_ELLIPSEMASK_H
