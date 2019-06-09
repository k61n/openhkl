//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/mask/IMask.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_MASK_IMASK_H
#define CORE_MASK_IMASK_H

namespace nsx {

class Ellipsoid;

//! Abstract class for detector mask
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

} // namespace nsx

#endif // CORE_MASK_IMASK_H
