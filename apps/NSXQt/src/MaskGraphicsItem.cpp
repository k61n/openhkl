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
  _diagonal()
{
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
   painter->drawRect(_diagonal.boundingRect());
}

QRectF MaskGraphicsItem::boundingRect() const
{
    return _diagonal.boundingRect();
}

void MaskGraphicsItem::setFrom(const QPointF& pos)
{
    _diagonal.setP1(pos);
    _diagonal.setP2(pos);
	QPointF bl=_diagonal.sceneBoundingRect().bottomLeft();
	QPointF tr=_diagonal.sceneBoundingRect().topRight();
	int nFrames=_data->getNFrames();
    _aabb->setLower({bl.x(),bl.y(),nFrames});
    _aabb->setUpper({tr.x(),tr.y(),nFrames});
    setPos(pos);
    update();
}

void MaskGraphicsItem::setTo(const QPointF& pos)
{
	_diagonal.setP2(pos);
	QPointF bl=_diagonal.sceneBoundingRect().bottomLeft();
	QPointF tr=_diagonal.sceneBoundingRect().topRight();
	int nFrames=_data->getNFrames();
    _aabb->setLower({bl.x(),bl.y(),nFrames});
    _aabb->setUpper({tr.x(),tr.y(),nFrames});
    setPos(0.5*(_diagonal.P1()+_diagonal.P2()));
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
    setFrom(tr+QPointF(step,step));
}
