//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/SXGraphicsItem.cpp
//! @brief     Implements class SXGraphicsItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/SXGraphicsItem.h"

#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

SXGraphicsItem::SXGraphicsItem(QGraphicsItem* parent, bool deletable, bool movable)
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

void SXGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    _hovered = true;
    setCursor(QCursor(Qt::PointingHandCursor));
    update();
}

void SXGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    _hovered = false;
    setCursor(QCursor(Qt::CrossCursor));
    update();
}

bool SXGraphicsItem::isInScene(const QPointF& pos) const
{
    QRectF rect = scene()->sceneRect();
    return (
        pos.x() > rect.left() && pos.x() < rect.right() && pos.y() > rect.top()
        && pos.y() < rect.bottom());
}

void SXGraphicsItem::setDeletable(bool deletable)
{
    _deletable = deletable;
}

void SXGraphicsItem::setMovable(bool movable)
{
    _movable = movable;
    setFlag(QGraphicsItem::ItemIsMovable, _movable);
}

bool SXGraphicsItem::isDeletable() const
{
    return _deletable;
}

bool SXGraphicsItem::isMovable() const
{
    return _movable;
}

void SXGraphicsItem::showLabel(bool show)
{
    _label_gi->setVisible(show);
}

void SXGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event)
}

void SXGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    Q_UNUSED(event)
}

void SXGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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
