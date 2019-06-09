//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      ###FILE###
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/graphics/plottableitem.h"
#include "gui/graphics/nsxplot.h"

PlottableItem::PlottableItem(QGraphicsItem* parent, bool deletable, bool movable)
    : NSXGraphicsItem(parent, deletable, movable)
{
}

PlottableItem::~PlottableItem() {}

bool PlottableItem::isPlottable(NSXPlot* plot) const
{
    if (!plot)
        return false;
    else
        return (getPlotType().compare(plot->getType()) == 0);
}
