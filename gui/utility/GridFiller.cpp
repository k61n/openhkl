//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
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
#include "gui/utility/Spoiler.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>


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


 GridFiller::~GridFiller() { 
     if (_spoiler != nullptr)
         _spoiler->setContentLayout(*_mainLayout, _spoiler->isExpanded());
 }

QComboBox* GridFiller::addCombo(const QString& labelText, const QString& labelTooltip)
 {
    addLabel(labelText, labelTooltip);

    QComboBox* comboBox = new QComboBox();
    comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _mainLayout->addWidget(comboBox, _nextRow, 1, 1, -1);

    _nextRow++;

    return comboBox;
}

QCheckBox* GridFiller::addCheckBox(const QString& title, int col)
{
    auto checkBox = new QCheckBox(title);
    _mainLayout->addWidget(checkBox, _nextRow, col, 1, -1);
    _nextRow++;

    return checkBox;
}

QDoubleSpinBox* GridFiller::addDoubleSpinBox(const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    auto* spinBox = new QDoubleSpinBox();
    _mainLayout->addWidget(spinBox, _nextRow, 1, 1, -1);

    _nextRow++;
    return spinBox;
}

QSpinBox* GridFiller::addSpinBox(const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    QSpinBox* spinBox = new QSpinBox();
    _mainLayout->addWidget(spinBox, _nextRow, 1, 1, -1);

    _nextRow++;
    return spinBox;
}

std::tuple<QSpinBox*, QSpinBox*> GridFiller::addSpinBoxPair(
    const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    auto spinBox1 = new QSpinBox();
    _mainLayout->addWidget(spinBox1, _nextRow, 1, 1, 1);

    auto spinBox2 = new QSpinBox();
    _mainLayout->addWidget(spinBox2, _nextRow, 2, 1, 1);

    _nextRow++;
    return std::make_tuple(spinBox1, spinBox2);
}

std::tuple<QDoubleSpinBox*, QDoubleSpinBox*> GridFiller::addDoubleSpinBoxPair(
    const QString& labelText, const QString& labelTooltip)
{
    addLabel(labelText, labelTooltip);

    auto spinBox1 = new QDoubleSpinBox();
    _mainLayout->addWidget(spinBox1, _nextRow, 1, 1, 1);

    auto spinBox2 = new QDoubleSpinBox();
    _mainLayout->addWidget(spinBox2, _nextRow, 2, 1, 1);

    _nextRow++;
    return std::make_tuple(spinBox1, spinBox2);
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
