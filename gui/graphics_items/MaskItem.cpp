//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/MaskItem.cpp
//! @brief     Implements class MaskItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/MaskItem.h"

int MaskItem::_n_masks = 0;

MaskItem::MaskItem(ohkl::AABB* aabb)
    : SXGraphicsItem(nullptr, true, true)
    , _aabb(aabb)
    , _id(_n_masks++)
{
}
