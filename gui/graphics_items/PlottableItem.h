//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/PlottableItem.h
//! @brief     Defines class PlottableItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_ITEMS_PLOTTABLEITEM_H
#define NSX_GUI_GRAPHICS_ITEMS_PLOTTABLEITEM_H

#include "gui/graphics_items/SXGraphicsItem.h"

class SXPlot;

//! Base class for all graphics items which can be plotted. Inherits from SXGraphicsItem
class PlottableItem : public SXGraphicsItem {
 public:
    PlottableItem(QGraphicsItem* parent = 0, bool deletable = true, bool movable = false);
    virtual ~PlottableItem();

    virtual void plot(SXPlot* plot) = 0;

    // Getters and setters

    //! Returns the type of plot related to the item
    virtual std::string getPlotType() const = 0;

    // Other methods

    bool isPlottable(SXPlot* plot) const;
};

#endif // NSX_GUI_GRAPHICS_ITEMS_PLOTTABLEITEM_H
