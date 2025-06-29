//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/SafeSpinBox.h
//! @brief     Defines classes SafeSpinBox and SafeDoubleSpinBox
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_UTILITY_SAFESPINBOX_H
#define OHKL_GUI_UTILITY_SAFESPINBOX_H

#include <QDoubleSpinBox>
#include <QSpinBox>

//! A QSpinBox that doesn't accept wheel events
class SafeSpinBox : public QSpinBox {
    Q_OBJECT

 public:
    SafeSpinBox(QWidget* parent = nullptr);

 protected:
    virtual void wheelEvent(QWheelEvent* event);
};

class SafeDoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT

 public:
    SafeDoubleSpinBox(QWidget* parent = nullptr);

 protected:
    virtual void wheelEvent(QWheelEvent* event);
};

#endif // OHKL_GUI_UTILITY_SAFESPINBOX_H
