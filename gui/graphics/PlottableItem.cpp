//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/PlottableItem.cpp
//! @brief     Implements class PlottableItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/PlottableItem.h"

#include "gui/graphics/SXPlot.h"

PlottableItem::PlottableItem(QGraphicsItem* parent, bool deletable, bool movable)
    : SXGraphicsItem(parent, deletable, movable)
{
}

PlottableItem::~PlottableItem() {}

bool PlottableItem::isPlottable(SXPlot* plot) const
{
    if (!plot)
        return false;
    else
        return (getPlotType().compare(plot->getType()) == 0);
}
