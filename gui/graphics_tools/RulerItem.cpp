//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics_tools/RulerItem.cpp
//! @brief     Implements class RulerItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <sstream>

#include <QGraphicsTextItem>
#include <QPainter>
#include <QPen>

#include "gui/graphics_tools/RulerItem.h"

RulerItem::RulerItem(QGraphicsItem* parent) : QGraphicsItem(parent)
{
    // Create 3 lines and add them to the Group
    _base = new QGraphicsLineItem();
    _body = new QGraphicsLineItem();
    _tip = new QGraphicsLineItem();
    // Default tip width is 30 pixels
    _hwidth = 20;
    _basetext = new QGraphicsTextItem();
    _bodytext = new QGraphicsTextItem();
    _tiptext = new QGraphicsTextItem();
    //
    _start.setX(0);
    _start.setY(0);
    _end.setX(0);
    _end.setY(0);
    //
    _calibrated = false;
    _displaydistance = true;
}

RulerItem::~RulerItem() = default;

QRectF RulerItem::boundingRect() const
{
    // return bounding box between extreme points
    return QRect(_start.toPoint(), _end.toPoint());
}

void RulerItem::setTipWidth(double w)
{
    _hwidth = w / 2.0;
}

void RulerItem::startDrawingAt(const QPointF& pos)
{
    _start = pos;
    // Initially base and tip are simply horizontal lines.
    _base->setLine(pos.x() - _hwidth, pos.y(), pos.x() + _hwidth, pos.y());
    _body->setLine(pos.x(), pos.y(), pos.x(), pos.y());
    _tip->setLine(pos.x() - _hwidth, pos.y(), pos.x() + _hwidth, pos.y());
    std::ostringstream os;
    // Set up text
    os << "(" << pos.x() << "," << pos.y() << ")";
    _basetext->setPos(pos.x(), pos.y() - 20);
    _basetext->setPlainText(QString::fromStdString(os.str()));
}

void RulerItem::moveTipAt(const QPointF& pos)
{
    _end = pos;
    // Deviation angle with vertical
    double angle = atan2(pos.y() - _start.y(), pos.x() - _start.x());
    double cangle = cos(angle) * _hwidth;
    double sangle = sin(angle) * _hwidth;
    // Set the three lines
    _base->setLine(
        _start.x() + sangle, _start.y() - cangle, _start.x() - sangle, _start.y() + cangle);
    _body->setLine(_start.x(), _start.y(), pos.x(), pos.y());
    _tip->setLine(pos.x() + sangle, pos.y() - cangle, pos.x() - sangle, pos.y() + cangle);
    // Set up text
    std::ostringstream os;
    os << "(" << pos.x() << "," << pos.y() << ")";
    _tiptext->setPos(pos.x(), pos.y() + 20);
    _tiptext->setPlainText(QString::fromStdString(os.str()));

    if (_displaydistance && _calibrated) {
        double d1 = _end.x() - _start.x();
        double d2 = _end.y() - _start.y();
        double dd = sqrt(d1 * d1 + d2 * d2);
        os.str("");
        os << dd * _conversion << " mm";
        _bodytext->setPos(
            _start.x() + 0.5 * dd * cangle / _hwidth + 8,
            _start.y() + 0.5 * dd * sangle / _hwidth + 12);
        _bodytext->setRotation(angle * 180.0 / M_PI);
        _bodytext->setPlainText(QString::fromStdString(os.str()));
    }
}

void RulerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
    QPen pen(Qt::blue, 2, Qt::SolidLine);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawLine(_base->line());
    painter->drawLine(_body->line());
    painter->drawLine(_tip->line());
    painter->drawText(_basetext->pos(), _basetext->toPlainText());
    painter->drawText(_tiptext->pos(), _tiptext->toPlainText());
    painter->drawText(_bodytext->pos(), _bodytext->toPlainText());
}

QGraphicsItemGroup* RulerItem::stampRuler(double distance, const QPointF& pos)
{
    QGraphicsItemGroup* stamp = new QGraphicsItemGroup;
    QGraphicsLineItem* line = new QGraphicsLineItem;
    double d1 = _end.x() - _start.x();
    double d2 = _end.y() - _start.y();
    double dd = sqrt(d1 * d1 + d2 * d2);
    line->setPen(QPen(Qt::cyan));
    line->setLine(pos.x(), pos.y(), pos.x() + dd, pos.y());
    stamp->addToGroup(line);
    std::ostringstream os;
    os << distance << " (mm)";
    QGraphicsTextItem* text = new QGraphicsTextItem(QString::fromStdString(os.str()));
    text->setDefaultTextColor(Qt::white);
    text->setPos(pos + QPointF(0.5 * dd, -20));
    stamp->addToGroup(text);
    return stamp;
}

void RulerItem::setDistance(double val)
{
    double d1 = _end.x() - _start.x();
    double d2 = _end.y() - _start.y();
    double dd = sqrt(d1 * d1 + d2 * d2);
    _conversion = val / dd;
    _calibrated = true;
}

void RulerItem::activateDistanceDisplay()
{
    _displaydistance = true;
}

double RulerItem::getConversion() const
{
    return _conversion;
}
