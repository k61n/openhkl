//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/PlottableGraphicsItem.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include "apps/items/PlottableGraphicsItem.h"
#include "apps/models/DetectorScene.h"
#include "apps/plot/SXPlot.h"

PlottableGraphicsItem::PlottableGraphicsItem(QGraphicsItem* parent, bool deletable, bool movable)
    : SXGraphicsItem(parent, deletable, movable)
{
}

PlottableGraphicsItem::~PlottableGraphicsItem() {}

bool PlottableGraphicsItem::isPlottable(SXPlot* plot) const
{
    if (!plot)
        return false;
    else
        return (getPlotType().compare(plot->getType()) == 0);
}
