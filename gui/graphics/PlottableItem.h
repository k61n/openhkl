//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/PlottableItem.h
//! @brief     Defines class PlottableItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_PLOTTABLEITEM_H
#define GUI_GRAPHICS_PLOTTABLEITEM_H

#include "gui/graphics/SXGraphicsItem.h"

class NSXPlot;

class PlottableItem : public NSXGraphicsItem {
 public:
    PlottableItem(QGraphicsItem* parent = 0, bool deletable = true, bool movable = false);
    virtual ~PlottableItem();

    virtual void plot(NSXPlot* plot) = 0;

    // Getters and setters

    //! Returns the type of plot related to the item
    virtual std::string getPlotType() const = 0;

    // Other methods

    bool isPlottable(NSXPlot* plot) const;
};

#endif // GUI_GRAPHICS_PLOTTABLEITEM_H
