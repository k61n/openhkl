#include <algorithm>
#include <numeric>

#include <Eigen/Dense>

#include <QPainter>
#include <QGraphicsSceneWheelEvent>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include <nsxlib/data/DataSet.h>
#include <nsxlib/geometry/AABB.h>

#include "DetectorScene.h"
#include "items/MaskGraphicsItem.h"

MaskGraphicsItem::MaskGraphicsItem(nsx::sptrDataSet data, nsx::AABB* aabb)
: SXGraphicsItem(nullptr,true,true),
  _data(data),
  _aabb(aabb),
  _from(0,0),
  _to(0,0)
{
    _pen.setWidth(1);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
    _text=new QGraphicsTextItem(this);
    _text->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _text->setParentItem(this);
}

MaskGraphicsItem::~MaskGraphicsItem()
{
}

void MaskGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    painter->setBrush(QBrush(QColor(255,0,0,100)));

    // Color depending on selection
    if (option->state & QStyle::State_Selected)
        _pen.setStyle(Qt::DashLine);
    else
        _pen.setStyle(Qt::SolidLine);

    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->setPen(_pen);
    painter->drawRect(boundingRect());

    QPointF tl=sceneBoundingRect().topLeft();
    QPointF br=sceneBoundingRect().bottomRight();
    QString text2=QString::number(tl.x())+"\n"+QString::number(tl.y())+"\n"+QString::number(br.x())+"\n"+QString::number(br.y());

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
    setPos(pos);
    update();
    updateAABB();
}

void MaskGraphicsItem::setTo(const QPointF& pos)
{
    _to=pos;
    setPos(0.5*(_from+_to));
    update();
    updateAABB();
}

nsx::AABB* MaskGraphicsItem::getAABB()
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
        {
            SXGraphicsItem::mouseMoveEvent(event);
            updateAABB();
        }
    }
}

void MaskGraphicsItem::updateAABB()
{
    QPointF tl=sceneBoundingRect().topLeft();
    QPointF br=sceneBoundingRect().bottomRight();
    _aabb->setBounds({tl.x(),tl.y(),0},{br.x(),br.y(),static_cast<double>(_data->getNFrames())});
}

void MaskGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{

    // The item must be visible to be wheeled
    if (!isVisible())
        return;

    // The item must selected to be wheeled
    if (!isSelected())
        return;

    int step=event->delta()/120;

    if (_from.x() < _to.x())
    {
        if (_from.y() < _to.y())
        {
            _from += QPointF(-step,-step);
            _to += QPointF(step,step);
        }
        else
        {
            _from += QPointF(-step,step);
            _to += QPointF(step,-step);
        }
    }
    else
    {
        if (_from.y() < _to.y())
        {
            _from += QPointF(step,-step);
            _to += QPointF(-step,step);
        }
        else
        {
            _from += QPointF(step,step);
            _to += QPointF(-step,-step);
        }
    }

    update();

    updateAABB();

}
