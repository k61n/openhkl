#include <QGraphicsSceneWheelEvent>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <iostream>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QtDebug>
#include <QWidget>

#include "LineCutterCustomPlot.h"
#include "LineCutGraphicsItem.h"
#include "SXCustomPlot.h"

LineCutGraphicsItem::LineCutGraphicsItem(SX::Data::IData* data) : CutterGraphicsItem(data), _nPoints(10)
{
}

LineCutGraphicsItem::~LineCutGraphicsItem()
{
}

SXCustomPlot* LineCutGraphicsItem::createPlot(QWidget *parent)
{
    return new LineCutterCustomPlot(parent);
}

void LineCutGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(widget);

    // Color depending on selection
    if (option->state & QStyle::State_Selected)
       _pen.setStyle(Qt::DashLine);
    else
       _pen.setStyle(Qt::SolidLine);

    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->setPen(_pen);
    qreal w=_to.x()-_from.x();
    qreal h=_to.y()-_from.y();
    painter->drawLine(-w/2,-h/2,w/2,h/2);

}

int LineCutGraphicsItem::getNPoints() const
{
    return _nPoints;
}

void LineCutGraphicsItem::setNPoints(int nPoints)
{
    _nPoints = nPoints;
}

void LineCutGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (!isVisible())
        return;

    if (!isSelected())
        return;

    _nPoints += event->delta()>0 ? 1 : -1;
    if (_nPoints <= 0)
        _nPoints = 1;
}
