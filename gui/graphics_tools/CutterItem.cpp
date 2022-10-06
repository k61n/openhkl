//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_tools/CutterItem.cpp
//! @brief     Implements class CutterItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_tools/CutterItem.h"

#include <QGraphicsSceneMouseEvent>
#include <cmath>

CutterItem::CutterItem(ohkl::sptrDataSet data)
    : PlottableItem(nullptr, true, true), _data(data), _from(0, 0), _to(0, 0)
{
    setZValue(1);

    _pen.setWidth(1);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
}

CutterItem::~CutterItem() = default;

void CutterItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        // If the cutter is not selected, the move event corresponds to a
        // modification of the size of cutter
        if (!isSelected()) {
            if (isInScene(event->lastScenePos()))
                setTo(event->lastScenePos());
        }
        // Otherwise it is a standard move of the item
        else
            SXGraphicsItem::mouseMoveEvent(event);
    }
}

ohkl::sptrDataSet CutterItem::getData()
{
    return _data;
}

QRectF CutterItem::boundingRect() const
{
    qreal w = std::fabs(_to.x() - _from.x());
    qreal h = std::fabs(_to.y() - _from.y());
    return QRectF(-w / 2, -h / 2, w, h);
}

void CutterItem::setFrom(const QPointF& pos)
{
    _from = pos;
    _to = pos;
    setPos(_from);
    update();
}

void CutterItem::setTo(const QPointF& pos)
{
    _to = pos;
    setPos(0.5 * (_from + _to));
    update();
}
