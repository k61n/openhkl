//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/MaskItem.h
//! @brief     Defines class MaskItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_ITEMS_MASKITEM_H
#define OHKL_GUI_GRAPHICS_ITEMS_MASKITEM_H

#include "base/mask/IMask.h"
#include "gui/graphics_items/SXGraphicsItem.h"

//! Base class for the mask graphic items
class MaskItem : public SXGraphicsItem {
 public:
    MaskItem(ohkl::AABB* aabb);
    ~MaskItem() = default;

    //! Get the integer ID
    int id() const { return _id; };

    //! For sorting a vector of MaskItems
    bool operator<(const MaskItem& other) const { return _id < other._id; };

    //! Get the AABB
    virtual ohkl::AABB* getAABB() { return _aabb; };

    //! Get the actual mask
    virtual ohkl::IMask* mask() { return _mask; };

 protected:
    //! the AABB
    ohkl::AABB* _aabb;
    //! The integer ID
    int _id;
    //! Counter for assigning integer ID
    static int _n_masks;
    //! The mask
    ohkl::IMask* _mask;
};

#endif // OHKL_GUI_GRAPHICS_ITEMS_MASKITEM_H
