
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
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>

PeakViewWidget::PeakViewWidget(const QString& titleSet1, const QString& titleSet2)
{
    createSet(set1, titleSet1, Qt::green);
    addIntegrationRegion(set1, Qt::green, Qt::yellow);
    createSet(set2, titleSet2, Qt::red);
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
    connect(checkbox, &QCheckBox::stateChanged, this, &PeakViewWidget::settingsChanged);
    return checkbox;
}

QSpinBox* PeakViewWidget::addSpinBox(int row, int value)
{
    auto spinbox = new QSpinBox();
    spinbox->setValue(value);
    addWidget(spinbox, row, 1, 1, 1);
    connect(
        spinbox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &PeakViewWidget::settingsChanged);

    return spinbox;
}

QDoubleSpinBox* PeakViewWidget::addDoubleSpinBox(int row, double value)
{
    auto spinbox = new QDoubleSpinBox();
    spinbox->setValue(value);
    addWidget(spinbox, row, 1, 1, 1);
    connect(
        spinbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &PeakViewWidget::settingsChanged);

    return spinbox;
}

ColorButton* PeakViewWidget::addColorButton(int row, int col, const QColor& color)
{
    auto btn = new ColorButton(color);
    btn->setFixedSize(btn->sizeHint().height() * 2, btn->sizeHint().height());
    addWidget(btn, row, col, 1, 1);
    connect(btn, &ColorButton::colorChanged, this, &PeakViewWidget::settingsChanged);
    return btn;
}

void PeakViewWidget::createSet(Set& set, const QString& title, const QColor& btnColor)
{
    int row = rowCount();
    addHeadline(row++, title);

    addLabel(row, "Show:");
    set.drawPeaks = addCheckBox(row++, 1, "Centres", Qt::CheckState::Checked);

    addLabel(row, "Size:");
    set.sizePeaks = addSpinBox(row++, 10);

    addLabel(row, "Colour:");
    set.colorPeaks = addColorButton(row++, 1, btnColor);

    addHeadline(row++, "Bounding boxes:");

    addLabel(row, "Show:");
    set.drawBoxes = addCheckBox(row, 1, "Peak", Qt::CheckState::Unchecked);
    set.drawBkg = addCheckBox(row++, 2, "Background", Qt::CheckState::Unchecked);

    addLabel(row, "Colour:");
    set.colorBoxes = addColorButton(row, 1, btnColor);
    set.colorBkg = addColorButton(row, 2, btnColor);
}

void PeakViewWidget::addIntegrationRegion(Set& set, const QColor& peak, const QColor& bkg)
{
    int row = rowCount();
    addHeadline(row++, "Integration regions");

    addLabel(row, "Show:");
    set.drawIntegrationRegion =
        addCheckBox(row++, 1, "Integration region", Qt::CheckState::Unchecked);
    addLabel(row, "Alpha");
    set.alphaIntegrationRegion = addDoubleSpinBox(row++, 0.5);
    addLabel(row, "Colour");
    set.colorIntPeak = addColorButton(row, 1, peak);
    set.colorIntBkg = addColorButton(row++, 2, bkg);
}

void PeakViewWidget::Set::setColor(const QColor& color)
{
    colorPeaks->setColor(color);
    colorBoxes->setColor(color);
    colorBkg->setColor(color);
}

void PeakViewWidget::Set::setIntegrationRegionColors(
    const QColor& peak, const QColor& bkg, double alpha)
{
    colorIntPeak->setColor(peak);
    colorIntBkg->setColor(bkg);
    alphaIntegrationRegion->setValue(alpha);
}
