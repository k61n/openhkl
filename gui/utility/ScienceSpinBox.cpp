//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/ScienceSpinBox.h
//! @brief     Implements class ScienceSpinBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/utility/ScienceSpinBox.h"

#include "gui/utility/SafeSpinBox.h"

#include <QHBoxLayout>
#include <QLabel>

#include <cmath>

ScienceSpinBox::ScienceSpinBox(QWidget* parent) : QWidget(parent)
{
    QLabel* label = new QLabel("E");
    label->setFixedWidth(10);
    QHBoxLayout* layout = new QHBoxLayout(parent);
    _coefficient = new SafeDoubleSpinBox(this);
    _exponent = new SafeSpinBox(this);
    layout->addWidget(_coefficient);
    layout->addWidget(label);
    layout->addWidget(_exponent);

    _coefficient->setMaximum(10);
    _coefficient->setMinimum(-10);
    _exponent->setMaximum(20);
    _exponent->setMinimum(-20);
    setLayout(layout);
}

double ScienceSpinBox::value() const
{
    double coeff = _coefficient->value();
    double exp = _exponent->value();
    return coeff * std::pow(10, exp);
}

void ScienceSpinBox::setValue(double value)
{
    double exp = std::floor(std::log10(value));
    double coeff = value / std::pow(10, exp);
    _exponent->setValue(exp);
    _coefficient->setValue(coeff);
}
