//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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
    _currentColor = color;
    connect(this, &ColorButton::clicked, this, &ColorButton::chooseColor);
}

QColor ColorButton::getColor()
{
    return _currentColor;
}

void ColorButton::changeColor(const QColor& color)
{
    _currentColor = color;
    colorChanged(_currentColor);
}

void ColorButton::chooseColor()
{
    QColor color = QColorDialog::getColor(_currentColor, this);
    if (color.isValid() && color != _currentColor)
        changeColor(color);
}

void ColorButton::paintEvent(QPaintEvent* event)
{
    QPushButton::paintEvent(event);

    const int padding = 5;

    QPainter painter(this);
    painter.setBrush(QBrush(_currentColor));
    painter.setPen("#CECECE");
    painter.drawRect(rect().adjusted(padding, padding, -1 - padding, -1 - padding));
}