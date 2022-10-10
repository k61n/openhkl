//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/mask/IMask.h
//! @brief     Defines class IMask
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_MASK_IMASK_H
#define OHKL_BASE_MASK_IMASK_H

#include "base/geometry/AABB.h"

namespace ohkl {

class Ellipsoid;

enum class MaskType {
    Rectangle,
    Ellipse,
};

//! Pure virtual base class for detector mask.

class IMask {
 public:
    virtual ~IMask() = 0;

    virtual bool collide(const Ellipsoid& e) const = 0;

    virtual IMask* clone() const = 0;

    virtual const AABB& aabb() = 0;

    virtual void setAABB(const AABB& aabb) = 0;

    virtual bool isSelected() = 0;

    virtual void setSelected(bool selected) = 0;

 protected:
    IMask() = default;

    IMask(const IMask& other) = default;

    IMask& operator=(const IMask& other) = default;
};

} // namespace ohkl

#endif // OHKL_BASE_MASK_IMASK_H
