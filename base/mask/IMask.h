//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/mask/IMask.h
//! @brief     Defines class IMask
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_MASK_IMASK_H
#define NSX_BASE_MASK_IMASK_H

namespace ohkl {

class Ellipsoid;

//! Pure virtual base class for detector mask.

class IMask {
 public:
    virtual ~IMask() = 0;

    virtual bool collide(const Ellipsoid& e) const = 0;

    virtual IMask* clone() const = 0;

 protected:
    IMask() = default;

    IMask(const IMask& other) = default;

    IMask& operator=(const IMask& other) = default;
};

} // namespace ohkl

#endif // NSX_BASE_MASK_IMASK_H
