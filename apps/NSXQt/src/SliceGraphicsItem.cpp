#include "SliceGraphicsItem.h"
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <iostream>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>

SliceGraphicsItem::SliceGraphicsItem(SX::Data::IData* ptrData, bool horizontal):CutterGraphicsItem(nullptr)
{
    _data=ptrData;
    _horizontal=horizontal;
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    // Make sure it's on top
    setZValue(1);
    // Cosmetic, i.e. width is one in sceen coordinates
    _pen.setWidth(1);

}

void SliceGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(widget);

    // Color depending on selection
    if (option->state & QStyle::State_Selected)
    {
       _pen.setStyle(Qt::DashLine);
       painter->setBrush(QBrush(QColor(0,255,0,5)));
    }
    else
    {
       _pen.setStyle(Qt::SolidLine);
       _pen.setColor(QColor("black"));
    }

   painter->setRenderHint(QPainter::HighQualityAntialiasing);
   painter->setPen(_pen);
   int w=_x1-_x0;
   int h=_y1-_y0;
   painter->drawRect(-w/2.0,-h/2.0,w,h);

   if (_horizontal)
   {
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0+5,-h/2.0+2);
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0+5,-h/2.0-2);
       painter->drawLine(w/2.0,-h/2.0,w/2.0-5,-h/2.0+2);
       painter->drawLine(w/2.0,-h/2.0,w/2.0-5,-h/2.0-2);
   }
   else
   {
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0+2,-h/2.0+5);
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0-2,-h/2.0+5);
       painter->drawLine(-w/2.0, h/2.0,-w/2.0+2, h/2.0-5);
       painter->drawLine(-w/2.0, h/2.0,-w/2.0-2, h/2.0-5);
   }
}




