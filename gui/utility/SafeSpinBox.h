//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/utility/SafeSpinBox.h
//! @brief     Defines classes SafeSpinBox and SafeDoubleSpinBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_UTILITY_SAFESPINBOX_H
#define NSX_GUI_UTILITY_SAFESPINBOX_H

#include <QSpinBox>
#include <QDoubleSpinBox>

//! A QSpinBox that doesn't accept wheel events
class SafeSpinBox : public QSpinBox {
    Q_OBJECT

 public:
    SafeSpinBox(QWidget* parent = nullptr);

 protected:

    virtual void wheelEvent(QWheelEvent *event);
};

class SafeDoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT

 public:
    SafeDoubleSpinBox(QWidget* parent = nullptr);

 protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // NSX_GUI_UTILITY_SAFESPINBOX_H
