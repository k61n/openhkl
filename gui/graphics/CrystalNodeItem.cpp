//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/CrystalNodeItem.cpp
//! @brief     Implements class CrystalNodeItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/CrystalNodeItem.h"

CrystalNodeItem::CrystalNodeItem(QGraphicsItem* parent) : QGraphicsItem(parent)
{
    setZValue(10);
}

void CrystalNodeItem::initiate(double posx, double posy, double yc, double angle)
{
    _p1x = posx;
    _p1y = (posy - yc);
    // Assume the z position is same as y
    _p1z = -std::fabs(posy - yc);
    setPos(posx, posy);
    _angle = angle;
}
void CrystalNodeItem::adjust(double y, double yc, double angle)
{
    if (angle != _angle) {
        double anglerad = M_PI / 180.0 * (angle - _angle);
        _p1z = (-cos(anglerad) * _p1y + (y - yc)) / sin(anglerad);
    }
    rotate(angle, yc);
}
QRectF CrystalNodeItem::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-10 - penWidth / 2, -10 - penWidth / 2, 20 + penWidth, 20 + penWidth);
}

void CrystalNodeItem::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::cyan);
    painter->setBrush(QBrush(QColor(255, 0, 0, 120), Qt::SolidPattern));
    painter->drawRoundedRect(-4, -4, 8, 8, 2, 2);
}

void CrystalNodeItem::rotate(double angle, double yc)
{
    double anglerad = (angle - _angle) * M_PI / 180.0;
    double newy = cos(anglerad) * _p1y + sin(anglerad) * _p1z;
    setPos(_p1x, yc + newy);
}

void CrystalNodeItem::getCoordinates(double& x, double& y, double& z) const
{
    double anglerad = -_angle * M_PI / 180.0;
    x = _p1x;
    double ca = cos(anglerad);
    double sa = sin(anglerad);
    y = ca * _p1y + sa * _p1z;
    z = -sa * _p1y + ca * _p1z;
}
