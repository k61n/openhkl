#include <iostream>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QtDebug>

#include "CutterGraphicsItem.h"

CutterGraphicsItem::CutterGraphicsItem(SX::Data::IData* data) : PlottableGraphicsItem(nullptr), _data(data), _from(0,0), _to(0,0)
{
    setZValue(1);

    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

}

CutterGraphicsItem::~CutterGraphicsItem()
{
    scene()->removeItem(this);
}

void CutterGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {
        if (!isSelected())
        {
            if (isInScene(event->lastScenePos()))
                setTo(event->lastScenePos());
        }
        else
            PlottableGraphicsItem::mouseMoveEvent(event);
    }

}

SX::Data::IData* CutterGraphicsItem::getData()
{
    return _data;
}

QRectF CutterGraphicsItem::boundingRect() const
{
    qreal pw=_pen.width();
    qreal w=std::abs(_to.x()-_from.x());
    qreal h=std::abs(_to.y()-_from.y());
    return QRectF(-w/2,-h/2,w,h);
}

void CutterGraphicsItem::setFrom(const QPointF& pos)
{
    _from=pos;
    _to=pos;
    setPos(_from);
    update();
}

void CutterGraphicsItem::setTo(const QPointF& pos)
{
    _to=pos;
    setPos(0.5*(_from+_to));
    update();
}
