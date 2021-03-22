//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/PinItem.cpp
//! @brief     Implements class PinItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "gui/graphics_items/PinItem.h"

PinItem::PinItem(QGraphicsItem* parent) : QGraphicsItem(parent)
{
    _calibrated = false;
}
void PinItem::initiate(double posy, double angle)
{
    _p1y = posy;
    _p2y = posy;
    // Assume the z position is same as y;
    _angle = angle;
    _currentangle = angle;
    _calibrated = false;
}
void PinItem::adjust(double y, double angle)
{
    if (angle != _angle) {
        _p2y = y;
        _currentangle = angle;
        _calibrated = true;
    }
}
QRectF PinItem::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-10 - penWidth / 2, -10 - penWidth / 2, 20 + penWidth, 20 + penWidth);
}

void PinItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    if (_calibrated) {
        double anglerad = (_currentangle - _angle) * M_PI / 180.0;
        double newy = 0.5 * (_p1y + _p2y) + 0.5 * (_p1y - _p2y) * cos(anglerad);
        if (option->state & QStyle::State_Selected)
            painter->setPen(QColor(Qt::red));
        else
            painter->setPen(QColor(Qt::yellow));
        painter->drawLine(0, newy, 800, newy);
    } else
        painter->drawLine(0, _p1y, 800, _p1y);
}

void PinItem::rotate(double angle)
{
    _currentangle = angle;
}

bool PinItem::iscalibrated() const
{
    return _calibrated;
}

void PinItem::getCenter(double& y, double& z, double angle) const
{
    double anglerad = (angle - _angle) * M_PI / 180.0;
    y = 0.5 * (_p1y + _p2y) + 0.5 * (_p1y - _p2y) * cos(anglerad);
    z = 0.5 * (_p2y - _p1y) * sin(anglerad);
}
