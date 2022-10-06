//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/CrystalFaceItem.cpp
//! @brief     Implements class CrystalFaceItem
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>

#include <QPainter>

#include "gui/graphics/CrystalFaceItem.h"

CrystalFaceItem::CrystalFaceItem(QGraphicsItem* parent) : QGraphicsItem(parent)
{
    _points = new QPointF[3];
}

void CrystalFaceItem::set(
    const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const Eigen::Vector3d& p3)
{
    _points->setX(p1[0]);
    _points++;
    _points->setX(p2[0]);
    _points++;
    _points->setX(p3[0]);
    _points -= 2;
    _p1 = p1;
    _p2 = p2;
    _p3 = p3;
    Eigen::Vector3d ab = p2 - p1;
    Eigen::Vector3d ac = p3 - p1;
    _normal = ab.cross(ac);
}

void CrystalFaceItem::rotate(double angle, double yc)
{
    double anglerad = angle * M_PI / 180.0;
    double ca = cos(anglerad);
    double sa = sin(anglerad);
    _points->setY(ca * _p1[1] + sa * _p1[2] + yc);
    _points++;
    _points->setY(ca * _p2[1] + sa * _p2[2] + yc);
    _points++;
    _points->setY(ca * _p3[1] + sa * _p3[2] + yc);
    _points -= 2;
    nz = -_normal[1] * sa + _normal[2] * ca;
}
QRectF CrystalFaceItem::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-10 - penWidth / 2, -10 - penWidth / 2, 20 + penWidth, 20 + penWidth);
}
void CrystalFaceItem::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (nz < 0) {
        painter->setBrush(QBrush(QColor(0, 255, 255, 80)));
        painter->drawPolygon(_points, 3);
    }
}
