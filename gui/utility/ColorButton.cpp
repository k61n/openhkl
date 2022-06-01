//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/ColorButton.cpp
//! @brief     Implements class ColorButton
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/ColorButton.h"

#include <QColorDialog>

ColorButton::ColorButton(const QColor& color, QWidget* parent) : QPushButton(parent)
{
    setMinimumWidth(50);
    _color = color;
    connect(this, &ColorButton::clicked, this, &ColorButton::chooseColor);
}

QColor ColorButton::color()
{
    return _color;
}

void ColorButton::setColor(const QColor& color)
{
    _color = color;
    colorChanged(_color);
}

void ColorButton::chooseColor()
{
    QColor color = QColorDialog::getColor(_color, this);
    if (color.isValid() && color != _color)
        setColor(color);
}

void ColorButton::paintEvent(QPaintEvent* event)
{
    QPushButton::paintEvent(event);

    const int padding = 5;

    QPainter painter(this);
    painter.setBrush(QBrush(_color));
    painter.setPen("#CECECE");
    painter.drawRect(rect().adjusted(padding, padding, -1 - padding, -1 - padding));
}