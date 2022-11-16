//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/mask/EllipseMask.h
//! @brief     Defines class EllipseMask
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_MASK_ELLIPSEMASK_H
#define OHKL_BASE_MASK_ELLIPSEMASK_H

#include "base/geometry/AABB.h"
#include "base/geometry/Ellipsoid.h"
#include "base/mask/IMask.h"
#include "base/utils/Logger.h"

namespace ohkl {

//! Ellipsoidal detector IMask.

class EllipseMask : public IMask {
 public:
    EllipseMask() = delete;

    EllipseMask(const EllipseMask& other) = default;

    EllipseMask(const AABB& aabb, bool two_dim = true);

    ~EllipseMask() = default;

    EllipseMask& operator=(const EllipseMask& other) = default;

    IMask* clone() const override;

    bool collide(const Ellipsoid& ellipsoid) const override;

    const AABB& aabb() override { return _ellipsoid.aabb(); };

    void setAABB(const AABB& aabb) override;

 private:
    Ellipsoid _ellipsoid;
    bool _selected;
};

} // namespace ohkl

#endif // OHKL_BASE_MASK_ELLIPSEMASK_H
