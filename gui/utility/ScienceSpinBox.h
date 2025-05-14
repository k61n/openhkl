//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/ScienceSpinBox.h
//! @brief     Defines class ScienceSpinBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_SCIENCESPINBOX_H
#define OHKL_GUI_UTILITY_SCIENCESPINBOX_H

#include <QWidget>

class SafeDoubleSpinBox;
class SafeSpinBox;

class ScienceSpinBox : public QWidget {
    Q_OBJECT

 public:
    ScienceSpinBox(QWidget* parent = nullptr);

    double value() const;
    void setValue(double value);

 private:
    SafeDoubleSpinBox* _coefficient;
    SafeSpinBox* _exponent;
};

#endif // OHKL_GUI_UTILITY_SCIENCESPINBOX_H
