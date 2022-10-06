//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_tools/CutterItem.h
//! @brief     Defines class CutterItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_GRAPHICS_TOOLS_CUTTERITEM_H
#define OHKL_GUI_GRAPHICS_TOOLS_CUTTERITEM_H

#include "core/data/DataTypes.h"
#include "gui/graphics_items/PlottableItem.h"

//! Base class for data cutter items of the detector image
class CutterItem : public PlottableItem {
 public:
    //! Constructs a data cutter
    CutterItem(ohkl::sptrDataSet data);
    //! Destructor
    virtual ~CutterItem();

    // Events

    //! The mouse move event.
    //! If the item is selected when the event is triggered then the item will be
    //! moved on the scene Otherwise, that means that the item is being drawn and
    //! then the move event corresponds to a resize
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    // Getters and setters

    //! Returns the bounding rectangle of the item
    QRectF boundingRect() const override;
    //! Returns the data bound to the item
    ohkl::sptrDataSet getData();
    //! Sets the top left corner of the item
    void setFrom(const QPointF& pos);
    //! Sets the bottom right corner of the item
    void setTo(const QPointF& pos);

    const QPointF& from() const { return _from; }
    const QPointF& to() const { return _to; }

 protected:
    //! The data on which the cutter will act upon
    ohkl::sptrDataSet _data;
    //! The top left coordinates of the slice
    QPointF _from;
    //! The bottom right coordinates of the slice
    QPointF _to;
};

#endif // OHKL_GUI_GRAPHICS_TOOLS_CUTTERITEM_H
