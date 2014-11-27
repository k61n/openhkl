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

MaskGraphicsItem::MaskGraphicsItem(SX::Data::IData* data) : SXGraphicsItem(), _data(data), _from(0,0), _to(0,0), _boundaries()
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
    _to=_from;
    _boundaries.setUpper(Eigen::Vector3d(_to.x(),_to.y(),static_cast<double>(_data->getNFrames())));
    _boundaries.setLower(Eigen::Vector3d(_from.x(),_from.y(),static_cast<double>(_data->getNFrames())));
    setPos(pos);
    update();
}

void MaskGraphicsItem::setTo(const QPointF& pos)
{
    _to=pos;
    if (_to.x()>=_from.x())
    {
        if (_to.y() >= _from.y())
            _boundaries.setUpper(Eigen::Vector3d(_to.x(),_to.y(),static_cast<double>(_data->getNFrames())));
        else
        {
            _boundaries.setUpper(Eigen::Vector3d(_to.x(),_from.y(),static_cast<double>(_data->getNFrames())));
            _boundaries.setLower(Eigen::Vector3d(_from.x(),_to.y(),static_cast<double>(_data->getNFrames())));
        }
    }
    else
    {
        if (_to.y() >= _from.y())
        {
            _boundaries.setUpper(Eigen::Vector3d(_from.x(),_to.y(),static_cast<double>(_data->getNFrames())));
            _boundaries.setLower(Eigen::Vector3d(_to.x(),_from.y(),static_cast<double>(_data->getNFrames())));
        }
        else
            _boundaries.setLower(Eigen::Vector3d(_to.x(),_to.y(),static_cast<double>(_data->getNFrames())));
    }

    setPos(0.5*(_from+_to));
    update();

    excludePeaks();

}

void MaskGraphicsItem::excludePeaks() const
{
    auto& peaks=_data->getPeaks();

    for (auto peak : peaks)
        peak->setSelected(!_boundaries.intercept(*(peak->getBackground())));

}
