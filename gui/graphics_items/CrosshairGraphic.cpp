//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/CrosshairGraphic.cpp
//! @brief     Implements class CrosshairGraphic
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/CrosshairGraphic.h"

#include <QPainter>
#include <qgraphicsitem.h>

CrosshairGraphic::CrosshairGraphic(QPointF centre)
    : _size(15), _linewidth(1)
{
    setPos(centre);
    _pen.setColor(Qt::black);
    _pen.setWidth(_linewidth);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

    _xhair = new QGraphicsPathItem(this);

    _xhair->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _xhair->setFlag(QGraphicsItem::ItemIsMovable, true);
    _xhair->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _xhair->setAcceptHoverEvents(false);
    _xhair->setVisible(true);
    _xhair->setPen(_pen);
    _xhair->setZValue(20);
    redraw();
}

void CrosshairGraphic::redraw()
{
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(0, _size);
    path.lineTo(0, -_size);
    path.lineTo(0, 0);
    path.lineTo(_size, 0);
    path.lineTo(-_size, 0);
    path.lineTo(0, 0);

    _xhair->setPath(path);
}

QRectF CrosshairGraphic::boundingRect() const
{
    return {-_size / 2.0 - 1.0, -_size / 2.0 + 1.0, _size + 2.0, _size + 2.0};
}

void CrosshairGraphic::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)
    Q_UNUSED(painter)
}

void CrosshairGraphic::setSize(int size)
{
    _size = size;
    redraw();
}

void CrosshairGraphic::setLinewidth(int width)
{
    _linewidth = width;
    _pen.setWidth(_linewidth);
    redraw();
}

void CrosshairGraphic::show(bool s)
{
    _xhair->setVisible(s);
    redraw();
}
