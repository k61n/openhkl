//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/ellipseitem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_GRAPHICS_ELLIPSEITEM_H
#define GUI_GRAPHICS_ELLIPSEITEM_H

#include "core/experiment/DataTypes.h"
#include "core/geometry/GeometryTypes.h"
#include "gui/graphics/nsxgraphicsitem.h"
#include <QPointF>
#include <QGraphicsTextItem>

class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

/*! Creates a mask that will be used to unselect/select peaks whether their
 * intercept or not the mask
 */
class EllipseMaskItem : public NSXGraphicsItem {
public:
    // Constructs a mask
    EllipseMaskItem(nsx::sptrDataSet data, nsx::AABB* aabb);
    //! The destructor
    ~EllipseMaskItem();

    // Events

    //! The mouse move event.
    //! If the item is selected when the event is triggered then the item will be
    //! moved on the scene Otherwise, that means that the item is being drawn and
    //! then the move event corresponds to a resize
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event);

    // Getters and setters

    //! Returns the bounding rectangle of the mask
    QRectF boundingRect() const;
    nsx::AABB* getAABB();
    //! Sets the starting corner of the mask
    void setFrom(const QPointF& pos);
    //! Sets the ending corner of the mask
    void setTo(const QPointF& pos);

    // Other methods

    //! Paint the slice
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

protected:
    //! The data on which the cutter will act upon
    nsx::sptrDataSet _data;
    //! The AABB of the peak
    nsx::AABB* _aabb;
    QPointF _from;
    QPointF _to;
    QGraphicsTextItem* _text;

private:
    void updateAABB();
};

#endif // GUI_GRAPHICS_ELLIPSEITEM_H
