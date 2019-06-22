//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/SXGraphicsItem.cpp
//! @brief     Implements class NSXGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/SXGraphicsItem.h"

#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

NSXGraphicsItem::NSXGraphicsItem(QGraphicsItem* parent, bool deletable, bool movable)
    : QGraphicsItem(parent)
    , _deletable(deletable)
    , _hovered(false)
    , _movable(movable)
    , _firstMove(true)
    , _lastPos()
    , _label_gi(nullptr)
{
    _pen.setWidth(1);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

    // By default a plottable graphics can be selected
    setFlags(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable, _movable);
    setAcceptHoverEvents(true);
}

NSXGraphicsItem::~NSXGraphicsItem() {}

void NSXGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    _hovered = true;
    setCursor(QCursor(Qt::PointingHandCursor));
    update();
}

void NSXGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    _hovered = false;
    setCursor(QCursor(Qt::CrossCursor));
    update();
}

bool NSXGraphicsItem::isInScene(const QPointF& pos) const
{
    QRectF rect = scene()->sceneRect();
    return (
        pos.x() > rect.left() && pos.x() < rect.right() && pos.y() > rect.top()
        && pos.y() < rect.bottom());
}

void NSXGraphicsItem::setDeletable(bool deletable)
{
    _deletable = deletable;
}

void NSXGraphicsItem::setMovable(bool movable)
{
    _movable = movable;
    setFlag(QGraphicsItem::ItemIsMovable, _movable);
}

bool NSXGraphicsItem::isDeletable() const
{
    return _deletable;
}

bool NSXGraphicsItem::isMovable() const
{
    return _movable;
}

void NSXGraphicsItem::showLabel(bool show)
{
    _label_gi->setVisible(show);
}

void NSXGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
}

void NSXGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    Q_UNUSED(event)
}

void NSXGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

    if (!_movable || !isVisible() || !isSelected())
        return;

    if (_firstMove)
        _firstMove = false;
    else {
        // The translation vector
        QPointF dr = event->lastScenePos() - _lastPos;
        QRectF itemRect = sceneBoundingRect();
        itemRect.translate(dr);

        // At target position the item must be fully inside the scene
        if (scene()->sceneRect().contains(itemRect.topLeft())
            && scene()->sceneRect().contains(itemRect.bottomRight())) {
            moveBy(dr.x(), dr.y());
        }
    }
    _lastPos = event->lastScenePos();
}
