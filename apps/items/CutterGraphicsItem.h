//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/items/CutterGraphicsItem.h
//! @brief     Defines class CutterGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QPen>
#include <QPointF>

#include "core/experiment/DataTypes.h"

#include "apps/items/PlottableGraphicsItem.h"

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QWidget;

class CutterGraphicsItem : public PlottableGraphicsItem {
public:
    // Constructors and destructor

    //! Constructs a data cutter
    CutterGraphicsItem(nsx::sptrDataSet data);
    //! Destructor
    virtual ~CutterGraphicsItem();

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
