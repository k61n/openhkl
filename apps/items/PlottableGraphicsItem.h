//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/PlottableGraphicsItem.h
//! @brief     Defines class PlottableGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>

#include "apps/items/SXGraphicsItem.h"

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsTextItem;
class QGraphicsSceneWheelEvent;
class QKeyEvent;
class QWidget;
class SXPlot;

class PlottableGraphicsItem : public SXGraphicsItem {
public:
    PlottableGraphicsItem(QGraphicsItem* parent = 0, bool deletable = true, bool movable = false);
    virtual ~PlottableGraphicsItem();

    virtual void plot(SXPlot* plot) = 0;

    // Getters and setters

    //! Returns the type of plot related to the item
    virtual std::string getPlotType() const = 0;

    // Other methods

    bool isPlottable(SXPlot* plot) const;
};
