//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics_items/PeakCenterGraphic.cpp
//! @brief     Implements class PeakCenterGraphic
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics_items/PeakCenterGraphic.h"

#include "base/utils/Units.h"

#include "gui/graphics/PeakPlot.h"
#include "gui/utility/ColorButton.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

PeakCenterGraphic::PeakCenterGraphic(const Eigen::Vector3d& center)
{
    setVisible(true);
    setCenter(center);
    _size = Eigen::Vector2d(10, 10);
    _show_label = false;
    _show_center = true;
    _center_color = Qt::black;
    redraw();
}

void PeakCenterGraphic::redraw()
{
    QString peak_label;

    _label_gi = new QGraphicsTextItem(this);
    _label_gi->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _label_gi->setParentItem(this);
    _label_gi->setPlainText(peak_label);
    _label_gi->setAcceptHoverEvents(false);
    _label_gi->setZValue(-1);
    _label_gi->setPos(3, 3);
    _label_gi->setVisible(_show_label);

    _center_gi = new QGraphicsEllipseItem(this);
    _center_gi->setRect(-_size[0] / 2, -_size[1] / 2, _size[0], _size[1]);
    _center_gi->setParentItem(this);
    _center_gi->setAcceptHoverEvents(false);
    _center_gi->setZValue(10);
    _center_gi->setVisible(_show_center);

    setBoundingRegionGranularity(0.0);

    // setColor(_center_color);
    setZValue(1);
}

void PeakCenterGraphic::setCenter(const Eigen::Vector3d& center)
{
    setPos(center[0], center[1]);
}

void PeakCenterGraphic::setSize(int size)
{
    _size = Eigen::Vector2d(size, size);
    _center_gi->setRect(-_size[0] / 2, -_size[1] / 2, _size[0], _size[1]);
}

QRectF PeakCenterGraphic::boundingRect() const
{
    double width = _size[0];
    double height = _size[1];
    return QRectF(-width / 2.0, -height / 2.0, width, height);
}

void PeakCenterGraphic::setColor(QColor color)
{
    _center_color = color;
    QPen center_pen;
    center_pen.setCosmetic(true);
    center_pen.setColor(_center_color);
    center_pen.setStyle(Qt::SolidLine);
    _center_gi->setPen(center_pen);
}

void PeakCenterGraphic::paint(
    QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)
    Q_UNUSED(painter)
}

void PeakCenterGraphic::showLabel(bool flag)
{
    _show_label = flag;
    _label_gi->setVisible(_show_label);
}

void PeakCenterGraphic::showArea(bool flag)
{
    _show_center = flag;
    _center_gi->setVisible(_show_center);
}
