//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/mask/BoxMask.h
//! @brief     Defines class BoxMask
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_MASK_BOXMASK_H
#define OHKL_BASE_MASK_BOXMASK_H

#include "base/geometry/AABB.h"
#include "base/mask/IMask.h"

namespace ohkl {

//! Detector IMask, constructed from an AABB (axis-aligned bounding box).

class BoxMask : public IMask {
 public:
    BoxMask() = delete;

    BoxMask(const BoxMask& other) = default;

    BoxMask(const AABB& aabb);

    ~BoxMask() = default;

    BoxMask& operator=(const BoxMask& other) = default;

    IMask* clone() const override;

    bool collide(const Ellipsoid& ellipsoid) const override;

    const AABB& aabb() { return _aabb; };

    void setAABB(const AABB& aabb) override { _aabb = aabb; }

 private:
    AABB _aabb;
    bool _selected;
};

} // namespace ohkl

#endif // OHKL_BASE_MASK_BOXMASK_H
