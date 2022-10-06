//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/PlottableItem.cpp
//! @brief     Implements class PlottableItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/PlottableItem.h"

#include "gui/graphics/SXPlot.h"

PlottableItem::PlottableItem(QGraphicsItem* parent, bool deletable, bool movable)
    : SXGraphicsItem(parent, deletable, movable)
{
}

bool PlottableItem::isPlottable(SXPlot* plot) const
{
    if (!plot)
        return false;
    return (getPlotType().compare(plot->getType()) == 0);
}
