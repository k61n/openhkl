//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/SliceRectGraphicsItem.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "SliceRectGraphicsItem.h"

SliceRectGraphicsItem::SliceRectGraphicsItem(
    qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, width, height, parent)
{
}
