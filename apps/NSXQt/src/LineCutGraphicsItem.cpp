#include "LineCutGraphicsItem.h"
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <iostream>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QtDebug>

LineCutGraphicsItem::LineCutGraphicsItem(SX::Data::IData* ptrData):CutterGraphicsItem(nullptr)
{

    _data=ptrData;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    // Make sure it's on top
    setZValue(1);
    // Cosmetic, i.e. width is one in sceen coordinates
    _pen.setWidth(1);
    _pen.setColor(QColor("black"));

}

void LineCutGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(widget);

    // Color depending on selection
    if (option->state & QStyle::State_Selected)
    {
       _pen.setStyle(Qt::DashLine);
    }
    else
    {
       _pen.setStyle(Qt::SolidLine);
    }

    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->setPen(_pen);
    qreal w=_x1-_x0;
    qreal h=_y1-_y0;
    painter->drawLine(-w/2,-h/2,w/2,h/2);

}
