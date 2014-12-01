#include <algorithm>
#include <numeric>

#include <Eigen/Dense>

#include <QPainter>
#include <QGraphicsSceneWheelEvent>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QtDebug>

#include "AABB.h"
#include "DetectorScene.h"
#include "IData.h"
#include "Peak3D.h"
#include "MaskGraphicsItem.h"

MaskGraphicsItem::MaskGraphicsItem(SX::Data::IData* data)
: SXGraphicsItem(nullptr,true,false,true),
  _data(data),
  _aabb(new AABB<double,3>),
  _from(0,0),
  _to(0,0)
{
    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
}

MaskGraphicsItem::~MaskGraphicsItem()
{
}

void MaskGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(widget);

    painter->setBrush(QBrush(QColor(255,0,0,140)));

    // Color depending on selection
    if (option->state & QStyle::State_Selected)
        _pen.setStyle(Qt::DashLine);
    else
        _pen.setStyle(Qt::SolidLine);

   painter->setRenderHint(QPainter::HighQualityAntialiasing);
   painter->setPen(_pen);
   qreal w=_to.x()-_from.x();
   qreal h=_to.y()-_from.y();
   painter->drawRect(-w/2.0,-h/2.0,w,h);
}

QRectF MaskGraphicsItem::boundingRect() const
{
    qreal w=std::abs(_to.x()-_from.x());
    qreal h=std::abs(_to.y()-_from.y());
    return QRectF(-w/2,-h/2,w,h);
}

void MaskGraphicsItem::setFrom(const QPointF& pos)
{
    _from=pos;
    _to=pos;
    double nFrames=_data->getNFrames();
    _aabb->setUpper({_to.x(),_to.y(),nFrames});
    _aabb->setLower({_from.x(),_from.y(),0});
    setPos(pos);
    update();
}

void MaskGraphicsItem::setTo(const QPointF& pos)
{

    _to=pos;

    double nFrames=_data->getNFrames();

    if (_from.x() < _to.x())
    {
        if (_from.y() < _to.y())
        {
            _aabb->setLower({_from.x(),_from.y(),0});
            _aabb->setUpper({_to.x(),_to.y(),nFrames});
        }
        else
        {
            _aabb->setLower({_from.x(),_to.y(),0});
            _aabb->setUpper({_to.x(),_from.y(),nFrames});
        }
    }
    else
    {
        if (_from.y() < _to.y())
        {
            _aabb->setLower({_to.x(),_from.y(),0});
            _aabb->setUpper({_from.x(),_to.y(),nFrames});
        }
        else
        {
            _aabb->setLower({_to.x(),_to.y(),0});
            _aabb->setUpper({_from.x(),_from.y(),nFrames});
        }
    }

    setPos(0.5*(_from+_to));
    update();
}

AABB<double,3>* MaskGraphicsItem::getAABB()
{
	return _aabb;
}

void MaskGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if (event->buttons() & Qt::LeftButton)
    {
    	// If the mask is not selected, the move event corresponds to a modification of the size of mask
        if (!isSelected())
        {
            if (isInScene(event->lastScenePos()))
                setTo(event->lastScenePos());
        }
        // Otherwise it is a standard move of the item
        else
            SXGraphicsItem::mouseMoveEvent(event);
    }
}

void MaskGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{

	// The item must be visible to be wheeled
    if (!isVisible())
        return;

    // The item must selected to be wheeled
    if (!isSelected())
        return;

    int step=std::abs(event->delta()/120);

    QPointF bl=sceneBoundingRect().bottomLeft();
    QPointF tr=sceneBoundingRect().topRight();

    setFrom(bl-QPointF(step,step));
    setTo(tr+QPointF(step,step));
}
