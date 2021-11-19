
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

#include "core/peak/IntegrationRegion.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/utility/ColorButton.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>

PeakViewWidget::PeakViewWidget(const QString& titleSet1, const QString& titleSet2)
    : _peak_end(3.0), _bkg_begin(3.0), _bkg_end(6.0)
{
    createSet(set1, titleSet1, Qt::green);
    addIntegrationRegion(set1, Qt::yellow, Qt::green);
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
    spinbox->setSingleStep(0.1);
    addWidget(spinbox, row, 1, 1, 1);
    connect(
        spinbox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &PeakViewWidget::settingsChanged);

    return spinbox;
}

LinkedComboBox* PeakViewWidget::addCombo(int row, ComboType combo_type)
{
    auto* combo = new LinkedComboBox(combo_type, gGui->sentinel);
    addWidget(combo, row, 1, 1, 1);
    connect(
        combo, static_cast<void (LinkedComboBox::*)(int)>(&LinkedComboBox::currentIndexChanged),
        this, &PeakViewWidget::settingsChanged);

    return combo;
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
}

void PeakViewWidget::addIntegrationRegion(Set& set, const QColor& peak, const QColor& bkg)
{
    int row = rowCount();
    addHeadline(row++, "Integration regions");

    addLabel(row, "Show:");
    set.drawIntegrationRegion =
        addCheckBox(row++, 1, "Integration region", Qt::CheckState::Unchecked);
    set.regionType =
        addCombo(row++, ComboType::RegionType);

    for (int i = 0; i < static_cast<int>(nsx::RegionType::Count); ++i)
        for (const auto& [key, val] : nsx::regionTypeDescription)
            if (i == static_cast<int>(key))
                set.regionType->addItem(QString::fromStdString(val));

    addLabel(row, "Alpha");
    set.alphaIntegrationRegion = addDoubleSpinBox(row++, 0.2);
    set.alphaIntegrationRegion->setMaximum(1.0);
    addLabel(row, "Colour");
    set.colorIntPeak = addColorButton(row, 1, peak);
    set.colorIntBkg = addColorButton(row++, 2, bkg);
    set.previewIntRegion =
        addCheckBox(row++, 1, "Preview Integration Region", Qt::CheckState::Unchecked);
    addLabel(row, "Peak end");
    set.peakEnd = addDoubleSpinBox(row++, _peak_end);
    set.peakEnd->setMaximum(50.0);
    addLabel(row, "Background begin");
    set.bkgBegin = addDoubleSpinBox(row++, _bkg_begin);
    set.bkgBegin->setMaximum(10.0);
    addLabel(row, "Background end");
    set.bkgEnd = addDoubleSpinBox(row++, _bkg_end);
    set.bkgEnd->setMaximum(10.0);
}

void PeakViewWidget::Set::setColor(const QColor& color)
{
    colorPeaks->setColor(color);
}

void PeakViewWidget::Set::setIntegrationRegionColors(
    const QColor& peak, const QColor& bkg, double alpha)
{
    colorIntPeak->setColor(peak);
    colorIntBkg->setColor(bkg);
    alphaIntegrationRegion->setValue(alpha);
}

void PeakViewWidget::Set::setIntegrationBounds(double peak_end, double bkg_begin, double bkg_end)
{
    peakEnd->setValue(peak_end);
    bkgBegin->setValue(bkg_begin);
    bkgEnd->setValue(bkg_end);
}
