//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_tools/CutSliceItem.h
//! @brief     Defines class CutSliceItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_GRAPHICS_TOOLS_CUTSLICEITEM_H
#define NSX_GUI_GRAPHICS_TOOLS_CUTSLICEITEM_H

#include "gui/graphics_tools/CutterItem.h"

//! Cutter item that cuts a horizontal or vertical slice
class CutSliceItem : public CutterItem {
 public:
    //! Construct a data slicer
    CutSliceItem(nsx::sptrDataSet, bool horizontal = true);
    //! The destructor
    ~CutSliceItem() = default;

    // Events

    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;

    // Getters and setters

    //! Returns the type of plot related to this item
    std::string getPlotType() const override;

    // Other methods

    //! Plot the item
    void plot(SXPlot*) override;
    //! Return true if the slice is horizontal.
    bool isHorizontal() const;
    //! Paint the slice
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

 private:
    //! Horizontal or vertical integration
    bool _horizontal;
};

#endif // NSX_GUI_GRAPHICS_TOOLS_CUTSLICEITEM_H
