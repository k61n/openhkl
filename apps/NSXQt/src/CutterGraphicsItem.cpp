#include "CutterGraphicsItem.h"
#include <QCursor>


CutterGraphicsItem::CutterGraphicsItem(QGraphicsItem *parent)
:QGraphicsItem(parent),
_x0(0),_y0(0),_x1(0),_y1(0)
{
    _pen.setWidth(1);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

     setAcceptHoverEvents(true);
}


QRectF CutterGraphicsItem::boundingRect() const
{
    qreal pw=_pen.width();
    qreal w=_x1-_x0;
    qreal h=_y1-_y0;
    return QRectF(-w/2-pw/2.0,-h/2-pw/2,w+pw/2.0,h+pw/2.0);
}

void CutterGraphicsItem::from(qreal x, qreal y)
{
    _x0=x;
    _y0=y;
    _x1=x;
    _y1=y;
    setPos(_x0,_y0);
    update();

}

void CutterGraphicsItem::to(qreal x, qreal y)
{
    _x1=x;
    _y1=y;
    setPos(0.5*(_x0+_x1),0.5*(_y0+_y1));
    update();

}

void CutterGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::PointingHandCursor));
}

void CutterGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(QCursor(Qt::CrossCursor));
}

