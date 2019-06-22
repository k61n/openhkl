//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/SliceRectGraphicsItem.h
//! @brief     Defines class SliceRectGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QGraphicsRectItem>

class SliceRectGraphicsItem : public QGraphicsRectItem {
 public:
    SliceRectGraphicsItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);
};
