//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/mask/BoxMask.h
//! @brief     Defines class BoxMask
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_MASK_BOXMASK_H
#define NSX_BASE_MASK_BOXMASK_H

#include "base/geometry/AABB.h"
#include "base/mask/IMask.h"

namespace nsx {

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

 private:
    AABB _aabb;
};

} // namespace nsx

#endif // NSX_BASE_MASK_BOXMASK_H
