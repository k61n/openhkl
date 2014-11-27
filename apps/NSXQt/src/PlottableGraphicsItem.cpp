#include <iostream>

#include <QCursor>
#include <QtDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include "DetectorScene.h"
#include "PlottableGraphicsItem.h"
#include "SXCustomPlot.h"

PlottableGraphicsItem::PlottableGraphicsItem(QGraphicsItem *parent)
: QGraphicsItem(parent),
  _deletable(false),
  _hoverable(true),
  _movable(true),
  _label(nullptr)
{

    _pen.setWidth(1);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

    // By default a plottable graphics can be selected and moved
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);

    // By default a plottable graphics accepts hove event.
    setAcceptHoverEvents(true);
}

PlottableGraphicsItem::~PlottableGraphicsItem()
{
}

void PlottableGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    _hoverable=true;
    setCursor(QCursor(Qt::PointingHandCursor));
    update();
}

void PlottableGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    _hoverable=false;
    setCursor(QCursor(Qt::CrossCursor));
    update();
}

bool PlottableGraphicsItem::isInScene(const QPointF& pos) const
{
    QRectF rect=scene()->sceneRect();

    return (pos.x()>rect.left() && pos.x()<rect.right() && pos.y() > rect.top() && pos.y() <rect.bottom());
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
}

void PlottableGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
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

    // The translation vector
    QPointF dr = event->lastScenePos() - pos();

    QRectF itemRect=sceneBoundingRect();
    itemRect.translate(dr);

    // At target position the item must be fully inside the scene
    if (scene()->sceneRect().contains(itemRect.topLeft()) && scene()->sceneRect().contains(itemRect.bottomRight()))
        moveBy(dr.x(),dr.y());

}

bool PlottableGraphicsItem::isPlottable(SXCustomPlot* plot) const
{

    if (!plot)
        return false;
    else
        return (getPlotType().compare(plot->getType())==0);
}
