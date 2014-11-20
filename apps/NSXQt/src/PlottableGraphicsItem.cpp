#include <iostream>

#include <QCursor>
#include <QtDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include "DetectorScene.h"
#include "PlottableGraphicsItem.h"

PlottableGraphicsItem::PlottableGraphicsItem(QGraphicsItem *parent)
: QGraphicsItem(parent),
  _deletable(false),
  _hover(true),
  _movable(true),
  _label(nullptr)
{
    _pen.setWidth(1);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

    // By default a plottable graphics accepts hove event.
    setAcceptHoverEvents(true);
}

PlottableGraphicsItem::~PlottableGraphicsItem()
{
}

void PlottableGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(QCursor(Qt::PointingHandCursor));
}

void PlottableGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    setCursor(QCursor(Qt::CrossCursor));
}

bool PlottableGraphicsItem::isInScene(const QPointF& pos) const
{
    QRectF sceneRect=scene()->sceneRect();
    QRectF itemRect=boundingRect();
    itemRect.moveCenter(pos);
    return sceneRect.contains(itemRect);
}

void PlottableGraphicsItem::setDeletable(bool deletable)
{
    _deletable = deletable;
}

void PlottableGraphicsItem::setMovable(bool movable)
{
    _movable = movable;
}

bool PlottableGraphicsItem::isDeletable() const
{
    return _deletable;
}

bool PlottableGraphicsItem::isMovable() const
{
    return _movable;
}

void PlottableGraphicsItem::showLabel(bool show)
{
    _label->setVisible(show);
}

void PlottableGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "Pressed event";
}

void PlottableGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{

    // The item must be movable ... to be moved
    if (!_movable)
        return;

    // The item must be visible to be moved
    if (!isVisible())
        return;

    // The item must be selected to be moved
    if (!isSelected())
        return;

    // At target position the item must be fully inside the scene
    if (!isInScene(event->lastScenePos()))
        return;

    // Update the position of the item
    QPointF dr = event->lastScenePos() - pos();
    moveBy(dr.x(),dr.y());

}

