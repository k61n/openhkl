//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/PlottableItem.h
//! @brief     Defines class PlottableItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_ITEMS_PLOTTABLEITEM_H
#define OHKL_GUI_GRAPHICS_ITEMS_PLOTTABLEITEM_H

#include "gui/graphics_items/SXGraphicsItem.h"

class SXPlot;

//! Base class for all graphics items which can be plotted. Inherits from SXGraphicsItem
class PlottableItem : public SXGraphicsItem {
 public:
    PlottableItem(QGraphicsItem* parent = 0, bool deletable = true, bool movable = false);
    virtual ~PlottableItem() = default;

    virtual void plot(SXPlot* plot) = 0;

    // Getters and setters

    //! Returns the type of plot related to the item
    virtual std::string getPlotType() const = 0;

    // Other methods

    bool isPlottable(SXPlot* plot) const;
};

#endif // OHKL_GUI_GRAPHICS_ITEMS_PLOTTABLEITEM_H
