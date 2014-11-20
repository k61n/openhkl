#include "SliceGraphicsItem.h"
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <iostream>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QWidget>
#include <QGraphicsSceneWheelEvent>

#include "SliceCutterCustomPlot.h"
#include "SXCustomPlot.h"

SliceGraphicsItem::SliceGraphicsItem(SX::Data::IData* data, bool horizontal) : CutterGraphicsItem(data), _horizontal(horizontal)
{
}

SliceGraphicsItem::~SliceGraphicsItem()
{
}

SXCustomPlot* SliceGraphicsItem::createPlot(QWidget *parent)
{
    return new SliceCutterCustomPlot(parent);
}

bool SliceGraphicsItem::isHorizontal() const
{
    return _horizontal;
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
   qreal w=_to.x()-_from.x();
   qreal h=_to.y()-_from.y();
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

void SliceGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{

    if (!isVisible())
        return;

    if (!isSelected())
        return;

    int step=event->delta()/120;

    QPointF from=pos()+boundingRect().bottomLeft();
    QPointF to=pos()+boundingRect().topRight();

    if (_horizontal)
    {
        from += QPointF(0,-step);
        to += QPointF(0,step);
    }
    else
    {
        from += QPointF(-step,0);
        to += QPointF(step,0);
    }
    setFrom(from);
    setTo(to);
}



