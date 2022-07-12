//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/GridFiller.cpp
//! @brief     Implements class GridFiller
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/GridFiller.h"

#include "gui/MainWin.h" // gGui
#include "gui/utility/CellComboBox.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/FoundPeakComboBox.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PredictedPeakComboBox.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/ShapeComboBox.h"
#include "gui/utility/Spoiler.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

GridFiller::GridFiller(QGridLayout* gridLayout)
    : _mainLayout(gridLayout), _spoiler(nullptr), _nextRow(0)
{
}

GridFiller::GridFiller(Spoiler* spoiler, bool expanded)
    : _mainLayout(nullptr), _spoiler(spoiler), _nextRow(0)
{
    if (QGridLayout* grid = dynamic_cast<QGridLayout*>(spoiler->contentLayout()); grid != nullptr)
        _mainLayout = grid;
    else {
        _mainLayout = new QGridLayout();
        spoiler->setContentLayout(*_mainLayout, expanded);
    }
}

GridFiller::~GridFiller()
{
    if (_spoiler != nullptr)
        _spoiler->setContentLayout(*_mainLayout, _spoiler->isExpanded());
}

QComboBox* GridFiller::addCombo(const QString& labelText, const QString& tooltip)
{
    const bool createLabel = !labelText.isEmpty();
    if (createLabel)
        addLabel(labelText, tooltip);

    QComboBox* comboBox = new QComboBox();
    if (!createLabel)
        comboBox->setToolTip(tooltip);
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _mainLayout->addWidget(comboBox, _nextRow, createLabel ? 1 : 0, 1, -1);

    _nextRow++;

    return comboBox;
}

DataComboBox* GridFiller::addDataCombo(const QString& labelText, const QString& tooltip)
{
    const bool createLabel = !labelText.isEmpty();
    if (createLabel)
        addLabel(labelText, tooltip);

    DataComboBox* comboBox = new DataComboBox();
    if (!createLabel)
        comboBox->setToolTip(tooltip);
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _mainLayout->addWidget(comboBox, _nextRow, createLabel ? 1 : 0, 1, -1);

    _nextRow++;

    return comboBox;
}

CellComboBox* GridFiller::addCellCombo(const QString& labelText, const QString& tooltip)
{
    const bool createLabel = !labelText.isEmpty();
    if (createLabel)
        addLabel(labelText, tooltip);

    CellComboBox* comboBox = new CellComboBox();
    if (!createLabel)
        comboBox->setToolTip(tooltip);
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _mainLayout->addWidget(comboBox, _nextRow, createLabel ? 1 : 0, 1, -1);

    _nextRow++;

    return comboBox;
}

PeakComboBox* GridFiller::addPeakCombo(
    ComboType type, const QString& labelText, const QString& tooltip)
{
    const bool createLabel = !labelText.isEmpty();
    if (createLabel)
        addLabel(labelText, tooltip);

    PeakComboBox* comboBox;
    if (type == ComboType::PeakCollection)
        comboBox = new PeakComboBox();
    else if (type == ComboType::FoundPeaks)
        comboBox = new FoundPeakComboBox();
    else if (type == ComboType::PredictedPeaks)
        comboBox = new PredictedPeakComboBox();
    if (!createLabel)
        comboBox->setToolTip(tooltip);
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _mainLayout->addWidget(comboBox, _nextRow, createLabel ? 1 : 0, 1, -1);

    _nextRow++;

    return comboBox;
}

ShapeComboBox* GridFiller::addShapeCombo(const QString& labelText, const QString& tooltip)
{
    const bool createLabel = !labelText.isEmpty();
    if (createLabel)
        addLabel(labelText, tooltip);

    ShapeComboBox* comboBox = new ShapeComboBox();
    if (!createLabel)
        comboBox->setToolTip(tooltip);
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _mainLayout->addWidget(comboBox, _nextRow, createLabel ? 1 : 0, 1, -1);

    _nextRow++;

    return comboBox;
}

LinkedComboBox* GridFiller::addLinkedCombo(
    ComboType comboType, const QString& labelText, const QString& tooltip)
{
    const bool createLabel = !labelText.isEmpty();
    if (createLabel)
        addLabel(labelText, tooltip);

    LinkedComboBox* comboBox = new LinkedComboBox(comboType, gGui->sentinel);
    if (!createLabel)
        comboBox->setToolTip(tooltip);
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _mainLayout->addWidget(comboBox, _nextRow, createLabel ? 1 : 0, 1, -1);

    _nextRow++;

    return comboBox;
}

QLineEdit* GridFiller::addLineEdit(
    const QString& labelText, const QString& defaultText, const QString& tooltip)
{
    addLabel(labelText, tooltip);

    auto* line_edit = new QLineEdit();
    line_edit->setText(defaultText);
    _mainLayout->addWidget(line_edit, _nextRow, 1, 1, -1);

    _nextRow++;
    return line_edit;
}

QCheckBox* GridFiller::addCheckBox(const QString& title, int col)
{
    return addCheckBox(title, QString(), col);
}

QCheckBox* GridFiller::addCheckBox(const QString& title, const QString& tooltip, int col)
{
    auto* checkBox = new QCheckBox(title);
    checkBox->setToolTip(tooltip);
    _mainLayout->addWidget(checkBox, _nextRow, col, 1, -1);
    _nextRow++;

    return checkBox;
}

SafeDoubleSpinBox* GridFiller::addDoubleSpinBox(
    const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    auto* spinBox = new SafeDoubleSpinBox();
    spinBox->setSingleStep(0.1);
    spinBox->setFocusPolicy(Qt::ClickFocus);
    _mainLayout->addWidget(spinBox, _nextRow, 1, 1, -1);

    _nextRow++;
    return spinBox;
}

SafeSpinBox* GridFiller::addSpinBox(const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    SafeSpinBox* spinBox = new SafeSpinBox();
    spinBox->setFocusPolicy(Qt::ClickFocus);
    _mainLayout->addWidget(spinBox, _nextRow, 1, 1, -1);

    _nextRow++;
    return spinBox;
}

std::tuple<SafeSpinBox*, SafeSpinBox*> GridFiller::addSpinBoxPair(
    const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    auto* spinBox1 = new SafeSpinBox();
    _mainLayout->addWidget(spinBox1, _nextRow, 1, 1, 1);

    auto* spinBox2 = new SafeSpinBox();
    _mainLayout->addWidget(spinBox2, _nextRow, 2, 1, 1);

    _nextRow++;
    return std::make_tuple(spinBox1, spinBox2);
}

std::tuple<SafeDoubleSpinBox*, SafeDoubleSpinBox*> GridFiller::addDoubleSpinBoxPair(
    const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    auto* spinBox1 = new SafeDoubleSpinBox();
    _mainLayout->addWidget(spinBox1, _nextRow, 1, 1, 1);

    auto* spinBox2 = new SafeDoubleSpinBox();
    _mainLayout->addWidget(spinBox2, _nextRow, 2, 1, 1);

    _nextRow++;
    return std::make_tuple(spinBox1, spinBox2);
}

QPushButton* GridFiller::addButton(const QString& text, const QString& tooltip)
{
    auto* button = new QPushButton(text);
    button->setToolTip(tooltip);
    _mainLayout->addWidget(button, _nextRow, 1, 1, -1);

    _nextRow++;
    return button;
}

ColorButton* GridFiller::addColorButton(
    const QColor& color, const QString& labelText, const QString& labelTooltip /* = QString() */)
{
    addLabel(labelText, labelTooltip);
    auto* btn = new ColorButton(color);
    btn->setFixedSize(btn->sizeHint().height() * 2, btn->sizeHint().height());
    _mainLayout->addWidget(btn, _nextRow, 1, 1, -1);

    _nextRow++;
    return btn;
}

void GridFiller::addWidget(QWidget* w, int col, int colspan)
{
    _mainLayout->addWidget(w, _nextRow, col, 1, colspan);

    _nextRow++;
}

void GridFiller::addLabel(const QString& labelText, const QString& labelTooltip)
{
    QString t = labelText.trimmed();
    if (!t.endsWith(":"))
        t.append(":");
    QLabel* label = new QLabel(t);
    label->setToolTip(labelTooltip);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _mainLayout->addWidget(label, _nextRow, 0, 1, 1);
}
