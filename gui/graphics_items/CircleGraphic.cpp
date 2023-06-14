//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_items/CircleGraphic.cpp
//! @brief     Implements class CircleGraphic
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/CircleGraphic.h"

#include <QPainter>
#include <qgraphicsitem.h>

CircleGraphic::CircleGraphic(const QPointF& centre, int radius)
    : _circle(new QGraphicsEllipseItem(this))
    , _peak(new QGraphicsEllipseItem(this))
    , _label(new QGraphicsTextItem(this))
    , _radius(radius)
    , _peak_radius(5)
    , _linewidth(1)
{
    setPos(centre);
    _pen.setColor(Qt::black);
    _pen.setWidth(_linewidth);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);

    _circle->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _circle->setFlag(QGraphicsItem::ItemIsMovable, true);
    _circle->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _circle->setAcceptHoverEvents(false);
    _circle->setVisible(true);
    _circle->setPen(_pen);
    _circle->setZValue(20);

    _peak->setFlag(QGraphicsItem::ItemIsSelectable, true);
    _peak->setFlag(QGraphicsItem::ItemIsMovable, true);
    _peak->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    _peak->setAcceptHoverEvents(false);
    _peak->setVisible(true);
    _peak->setPen(_pen);
    _peak->setZValue(20);

    _label->setDefaultTextColor(Qt::black);
    _label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _label->setAcceptHoverEvents(false);
    _label->setZValue(20);
    _label->setPos(3, 3);
    _label->adjustSize();

    redraw();
}

void CircleGraphic::redraw()
{
    _circle->setRect(-_radius, -_radius, 2.0 * _radius, 2.0 * _radius);
    _peak->setRect(-_peak_radius, -_peak_radius, 2.0 * _peak_radius, 2.0 * _peak_radius);
}

QRectF CircleGraphic::boundingRect() const
{
    return {
        -_radius / 2.0 - _linewidth / 2.0, -_radius / 2.0 + _linewidth / 2.0,
        static_cast<double>(_radius + _linewidth), static_cast<double>(_radius + _linewidth)};
}

void CircleGraphic::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)
    Q_UNUSED(painter)
}

void CircleGraphic::setRadius(int radius)
{
    _radius = radius;
    redraw();
}

void CircleGraphic::setLinewidth(int width)
{
    _linewidth = width;
    _pen.setWidth(_linewidth);
    _circle->setPen(_pen);
    redraw();
}

void CircleGraphic::show(bool s)
{
    _circle->setVisible(s);
    redraw();
}

void CircleGraphic::setLabel(const QString &text)
{
    _label->setPlainText(text);
    _label->setTextWidth(-1);
    _label->setVisible(true);
}
