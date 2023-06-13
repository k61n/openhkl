
//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/widgets/PeakViewWidget.cpp
//! @brief     Implements class PeakViewWidget
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/PeakViewWidget.h"

#include "core/peak/IntegrationRegion.h"
#include "gui/MainWin.h" // gGui
#include "gui/utility/ColorButton.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QSpinBox>

PeakViewWidget::PeakViewWidget(const QString& titleSet1, const QString& titleSet2)
{
    createSet(set1, titleSet1, Qt::green);
    addIntegrationRegion(set1, Qt::yellow, Qt::green);
    createSet(set2, titleSet2, Qt::red);
    connect(
        set1.regionType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &PeakViewWidget::switchIntRegionType);
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

QComboBox* PeakViewWidget::addCombo(int row)
{
    auto* combo = new QComboBox;
    addWidget(combo, row, 1, 1, 1);
    connect(
        combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &PeakViewWidget::settingsChanged);

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

    addLabel(row, "Show:");
    set.drawPeaks = addCheckBox(row++, 1, title, Qt::CheckState::Checked);

    addLabel(row, "Size:");
    set.sizePeaks = addSpinBox(row++, 10);

    addLabel(row, "Colour:");
    set.colorPeaks = addColorButton(row++, 1, btnColor);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    addWidget(line, row++, 0, 1, 3);
}

void PeakViewWidget::addIntegrationRegion(Set& set, const QColor& peak, const QColor& bkg)
{
    int row = rowCount();

    addLabel(row, "Show:");
    set.drawIntegrationRegion =
        addCheckBox(row++, 1, "Integration region", Qt::CheckState::Unchecked);

    addLabel(row, "Alpha:");
    set.alphaIntegrationRegion = addDoubleSpinBox(row++, 0.2);
    set.alphaIntegrationRegion->setMaximum(1.0);
    addLabel(row, "Peak colour:");
    set.colorIntPeak = addColorButton(row++, 1, peak);
    addLabel(row, "Background colour:");
    set.colorIntBkg = addColorButton(row++, 1, bkg);
    set.previewIntRegion = addCheckBox(row++, 1, "Preview new parameters", Qt::CheckState::Checked);
    addLabel(row, "Integration region type");
    set.regionType = addCombo(row++);
    addLabel(row, "Peak end:");
    set.peakEnd = addDoubleSpinBox(row++, set.params.peak_end);
    set.peakEnd->setMaximum(50.0);
    addLabel(row, "Background begin:");
    set.bkgBegin = addDoubleSpinBox(row++, set.params.bkg_begin);
    set.bkgBegin->setMaximum(10.0);
    addLabel(row, "Background end:");
    set.bkgEnd = addDoubleSpinBox(row++, set.params.bkg_end);
    set.bkgEnd->setMaximum(10.0);

    for (int i = 0; i < static_cast<int>(ohkl::RegionType::Count); ++i) {
        std::string description = ohkl::regionTypeDescription.at(static_cast<ohkl::RegionType>(i));
        set.regionType->addItem(QString::fromStdString(description));
    }

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    addWidget(line, row++, 0, 1, 3);

    connect(set.previewIntRegion, &QCheckBox::stateChanged, this, &PeakViewWidget::togglePreview);
}
void PeakViewWidget::togglePreview()
{
    set1.regionType->setDisabled(true);
    set1.peakEnd->setDisabled(true);
    set1.bkgBegin->setDisabled(true);
    set1.bkgEnd->setDisabled(true);
    if (set1.previewIntRegion->isChecked()) {
        set1.regionType->setDisabled(false);
        set1.peakEnd->setDisabled(false);
        set1.bkgBegin->setDisabled(false);
        set1.bkgEnd->setDisabled(false);
    }
}

void PeakViewWidget::switchIntRegionType()
{
    if (static_cast<ohkl::RegionType>(set1.regionType->currentIndex())
        == ohkl::RegionType::VariableEllipsoid) {
        set1.params.fixed_peak_end = set1.peakEnd->value();
        set1.params.fixed_bkg_begin = set1.bkgBegin->value();
        set1.params.fixed_bkg_end = set1.bkgEnd->value();
        set1.peakEnd->setValue(set1.params.peak_end);
        set1.bkgBegin->setValue(set1.params.bkg_begin);
        set1.bkgEnd->setValue(set1.params.bkg_end);
        set1.peakEnd->setSingleStep(0.1);
    } else {
        set1.params.peak_end = set1.peakEnd->value();
        set1.params.bkg_begin = set1.bkgBegin->value();
        set1.params.bkg_end = set1.bkgEnd->value();
        set1.peakEnd->setValue(set1.params.fixed_peak_end);
        set1.bkgBegin->setValue(set1.params.fixed_bkg_begin);
        set1.bkgEnd->setValue(set1.params.fixed_bkg_end);
        set1.peakEnd->setSingleStep(1.0);
    }
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
