
//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/PeakViewWidget.cpp
//! @brief     Implements class PeakViewWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/PeakViewWidget.h"
#include "gui/utility/ColorButton.h"

#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>

PeakViewWidget::PeakViewWidget(const QString& type1, const QString& type2)
{
    addHeadline(0, type1);

    addLabel(1, "Show:");
    drawPeaks1 = addCheckBox(1, 1, "Centres", Qt::CheckState::Checked);

    addLabel(2, "Size:");
    sizePeaks1 = addSpinBox(2, 10);

    addLabel(3, "Colour:");
    colorPeaks1 = addColorButton(3, 1, Qt::darkGreen);

    addHeadline(4, "Bounding boxes:");

    addLabel(5, "Show:");
    drawBoxes1 = addCheckBox(5, 1, "Peak", Qt::CheckState::Unchecked);
    drawBkg1 = addCheckBox(5, 2, "Background", Qt::CheckState::Unchecked);

    addLabel(6, "Colour:");
    colorBoxes1 = addColorButton(6, 1, Qt::darkGreen);
    colorBkg1 = addColorButton(6, 2, Qt::darkGreen);

    addHeadline(7, type2);

    addLabel(8, "Show:");
    drawPeaks2 = addCheckBox(8, 1, "Centres", Qt::CheckState::Checked);

    addLabel(9, "Size:");
    sizePeaks2 = addSpinBox(9, 10);

    addLabel(10, "Colour:");
    colorPeaks2 = addColorButton(10, 1, Qt::darkRed);

    addHeadline(11, "Bounding boxes:");
    addLabel(12, "Show:");
    drawBoxes2 = addCheckBox(12, 1, "Peak", Qt::CheckState::Unchecked);
    drawBkg2 = addCheckBox(12, 2, "Background", Qt::CheckState::Unchecked);

    addLabel(13, "Colour:");
    colorBoxes2 = addColorButton(13, 1, Qt::darkRed);
    colorBkg2 = addColorButton(13, 2, Qt::darkRed);

    connect(drawPeaks1, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawPeaks2, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBoxes1, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBoxes2, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBkg1, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(drawBkg2, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorPeaks1, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorPeaks2, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBoxes1, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBoxes2, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBkg1, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    connect(colorBkg2, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);

    connect(
        sizePeaks1, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &PeakViewWidget::settingsChanged);
    connect(
        sizePeaks2, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &PeakViewWidget::settingsChanged);
}

void PeakViewWidget::addHeadline(int row, const QString& type)
{
    auto label = new QLabel(type);
    label->setAlignment(Qt::AlignLeft);
    addWidget(label, row, 0, 1, -1);
}

void PeakViewWidget::addLabel(int row, const QString& text)
{
    auto label = new QLabel(text);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    addWidget(label, row, 0, 1, 1);
}

QCheckBox* PeakViewWidget::addCheckBox(int row, int col, const QString& text, Qt::CheckState state)
{
    auto checkbox = new QCheckBox(text);
    checkbox->setCheckState(state);
    addWidget(checkbox, row, col);
    return checkbox;
}

QSpinBox* PeakViewWidget::addSpinBox(int row, int value)
{
    auto spinbox = new QSpinBox();
    spinbox->setValue(10);
    addWidget(spinbox, row, 1, 1, 1);
    return spinbox;
}

ColorButton* PeakViewWidget::addColorButton(int row, int col, const QColor& color)
{
    auto btn = new ColorButton(color);
    btn->setFixedSize(btn->sizeHint().height() * 2, btn->sizeHint().height());
    addWidget(btn, row, col, 1, 1);
    return btn;
}
