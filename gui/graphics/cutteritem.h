//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/cutteritem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_GRAPHICS_CUTTERITEM_H
#define GUI_GRAPHICS_CUTTERITEM_H

#include "core/experiment/DataTypes.h"
#include "gui/graphics/plottableitem.h"
class CutterItem : public PlottableItem {
public:
    // Constructors and destructor

    //! Constructs a data cutter
    CutterItem(nsx::sptrDataSet data);
    //! Destructor
    virtual ~CutterItem();

    // Events

    //! The mouse move event.
    //! If the item is selected when the event is triggered then the item will be
    //! moved on the scene Otherwise, that means that the item is being drawn and
    //! then the move event corresponds to a resize
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    // Getters and setters

    //! Returns the bounding rectangle of the item
    QRectF boundingRect() const;
    //! Returns the data bound to the item
    nsx::sptrDataSet getData();
    //! Sets the top left corner of the item
    void setFrom(const QPointF& pos);
    //! Sets the bottom right corner of the item
    void setTo(const QPointF& pos);

    const QPointF& from() const { return _from; }
    const QPointF& to() const { return _to; }

    // Other methods

    //! Paint the contents of the item [pure virtual]
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) = 0;

protected:
    //! The data on which the cutter will act upon
    nsx::sptrDataSet _data;
    //! The top left coordinates of the slice
    QPointF _from;
    //! The bottom right coordinates of the slice
    QPointF _to;
};

#endif // GUI_GRAPHICS_CUTTERITEM_H
