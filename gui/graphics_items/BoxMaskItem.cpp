//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/BoxMaskItem.cpp
//! @brief     Implements class BoxMaskItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/BoxMaskItem.h"

#include "base/mask/BoxMask.h"
#include "core/data/DataSet.h"
#include <Eigen/Dense>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>


BoxMaskItem::BoxMaskItem(ohkl::sptrDataSet data, ohkl::AABB* aabb)
    : MaskItem(aabb)
    , _data(data)
    , _from(0, 0)
    , _to(0, 0)
{
    _pen.setWidth(1);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
    _text = new QGraphicsTextItem(this);
    _text->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _text->setParentItem(this);
}

BoxMaskItem::~BoxMaskItem() = default;

void BoxMaskItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    // Color depending on selection
    if (option->state & QStyle::State_Selected) {
        _pen.setWidth(2);
        _pen.setColor(QColor(255, 0, 0, 255));
        _pen.setStyle(Qt::DashLine);
        painter->setBrush(QBrush(QColor(255, 0, 0, 100), Qt::BrushStyle(Qt::Dense5Pattern)));
    } else {
        _pen.setWidth(1);
        _pen.setColor(QColor(255, 0, 0, 255));
        _pen.setStyle(Qt::SolidLine);
        painter->setBrush(QBrush(QColor(255, 0, 0, 100), Qt::BrushStyle(Qt::SolidPattern)));
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(_pen);
    painter->drawRect(boundingRect());

    QPointF tl = sceneBoundingRect().topLeft();
    QPointF br = sceneBoundingRect().bottomRight();
    QString text2 = QString::number(tl.x()) + "\n" + QString::number(tl.y()) + "\n"
        + QString::number(br.x()) + "\n" + QString::number(br.y());
}

QRectF BoxMaskItem::boundingRect() const
{
    qreal w = std::abs(_to.x() - _from.x());
    qreal h = std::abs(_to.y() - _from.y());
    return QRectF(-w / 2, -h / 2, w, h);
}

void BoxMaskItem::setFrom(const QPointF& pos)
{
    _from = pos;
    _to = pos;
    setPos(pos);
    update();
    updateAABB();
}

void BoxMaskItem::setFrom(const Eigen::Vector3d& vec)
{
    _from = {vec(0), vec(1)};
    _to = {vec(0), vec(1)};
    setPos(_from);
    update();
    updateAABB();
}

void BoxMaskItem::setTo(const QPointF& pos)
{
    _to = pos;
    setPos(0.5 * (_from + _to));
    update();
    updateAABB();
}

void BoxMaskItem::setTo(const Eigen::Vector3d& vec)
{
    _to = {vec(0), vec(1)};
    setPos(0.5 * (_from + _to));
    update();
    updateAABB();
}

ohkl::AABB* BoxMaskItem::getAABB()
{
    return _aabb;
}

void BoxMaskItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        // If the mask is not selected, the move event corresponds to a modification
        // of the size of mask
        if (!isSelected()) {
            if (isInScene(event->lastScenePos()))
                setTo(event->lastScenePos());
        }
        // Otherwise it is a standard move of the item
        else {
            SXGraphicsItem::mouseMoveEvent(event);
            updateAABB();
        }
    }
}

void BoxMaskItem::updateAABB()
{
    QPointF tl = sceneBoundingRect().topLeft();
    QPointF br = sceneBoundingRect().bottomRight();
    _aabb->setLower({tl.x(), tl.y(), 0});
    _aabb->setUpper({br.x(), br.y(), static_cast<double>(_data->nFrames())});
}

void BoxMaskItem::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    // The item must be visible to be wheeled
    if (!isVisible())
        return;

    // The item must selected to be wheeled
    if (!isSelected())
        return;

    int step = event->delta() / 120;

    if (_from.x() < _to.x()) {
        if (_from.y() < _to.y()) {
            _from += QPointF(-step, -step);
            _to += QPointF(step, step);
        } else {
            _from += QPointF(-step, step);
            _to += QPointF(step, -step);
        }
    } else {
        if (_from.y() < _to.y()) {
            _from += QPointF(step, -step);
            _to += QPointF(-step, step);
        } else {
            _from += QPointF(step, step);
            _to += QPointF(-step, -step);
        }
    }

    update();

    updateAABB();
}
