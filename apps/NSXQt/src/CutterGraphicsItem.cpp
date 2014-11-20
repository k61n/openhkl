#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QtDebug>

#include "CutterGraphicsItem.h"

CutterGraphicsItem::CutterGraphicsItem(SX::Data::IData* data) : PlottableGraphicsItem(nullptr), _data(data), _from(0,0), _to(0,0)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);

    setZValue(1);

    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

}

CutterGraphicsItem::~CutterGraphicsItem()
{
}

void CutterGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    qDebug()<<"I ENTER IN CUTTER MOVE EVENT";

    if (event->buttons())
    {
        if (Qt::LeftButton)
        {
            qDebug()<<"I ENTER IN CUTTER MOVE EVENT WITH LEFT BUTTON CLICKED";
            if (!isInScene(event->lastScenePos()))
                return;
            setTo(event->lastScenePos());
        }
    }
    else
    {
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
    qreal w=_to.x()-_from.x();
    qreal h=_to.y()-_from.y();
    return QRectF(-w/2-pw/2.0,-h/2-pw/2,w+pw/2.0,h+pw/2.0);
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
